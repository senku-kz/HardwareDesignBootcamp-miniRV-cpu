// miniRV_cpu.sv
// Top-level miniRV CPU module

module miniRV_cpu (
    input  logic        clk,
    input  logic        rst_n,
    output logic [31:0] imem_addr,    // instruction memory address (byte address)
    input  logic [31:0] imem_rdata,    // instruction memory read data
    output logic [31:0] dmem_addr,    // data memory address (byte address)
    output logic [31:0] dmem_wdata,    // data memory write data
    output logic [3:0]  dmem_wstrb,    // data memory write strobe
    input  logic [31:0] dmem_rdata     // data memory read data
);

    // Internal signals
    logic [31:0] pc, pc_next, pc_plus4;
    logic [31:0] instr;
    logic [6:0]  opcode;
    /* verilator lint_off UNUSEDSIGNAL */
    logic [4:0]  rd, rs1, rs2;  // Only lower 4 bits used for x0-x15, but decode outputs full 5 bits
    /* verilator lint_on UNUSEDSIGNAL */
    logic [2:0]  funct3;
    logic [31:0] imm_i, imm_u;
    logic        illegal_reg;
    // Unused decode outputs (kept for completeness but not used in miniRV)
    /* verilator lint_off UNUSEDSIGNAL */
    logic [6:0]  funct7_unused;
    logic [31:0] imm_s_unused;
    /* verilator lint_on UNUSEDSIGNAL */

    // Register file signals
    logic        reg_write;
    logic [31:0] rs1_data, rs2_data;
    logic [31:0] wb_data;

    // ALU signals
    logic [31:0] alu_a, alu_b, alu_result;

    // Control signals
    logic        alu_src;
    logic [1:0]  wb_sel;
    logic        pc_src;
    logic        is_lbu, is_sb;
    logic        illegal_instr;
    /* verilator lint_off UNUSEDSIGNAL */
    logic        mem_read, mem_write;  // Output from control but not used (we use is_lbu/is_sb instead)
    /* verilator lint_on UNUSEDSIGNAL */

    // Memory read data processing
    logic [31:0] mem_rdata_processed;
    logic [1:0]  byte_offset;

    // PC register
    always_ff @(posedge clk or negedge rst_n) begin
        /* verilator lint_off SYNCASYNCNET */
        if (!rst_n) begin
        /* verilator lint_on SYNCASYNCNET */
            pc <= 32'h0;
        end else begin
            pc <= pc_next;
        end
    end

    // PC update logic
    assign pc_plus4 = pc + 32'h4;
    assign imem_addr = pc;
    assign instr = imem_rdata;

    // Instruction decode
    decode u_decode (
        .instr(instr),
        .opcode(opcode),
        .rd(rd),
        .funct3(funct3),
        .rs1(rs1),
        .rs2(rs2),
        .funct7(funct7_unused),  // Not used in miniRV
        .imm_i(imm_i),
        .imm_s(imm_s_unused),    // Not used in miniRV (stores use imm_i)
        .imm_u(imm_u),
        .illegal_reg(illegal_reg)
    );

    // Control unit
    control u_control (
        .opcode(opcode),
        .funct3(funct3),
        .illegal_reg(illegal_reg),
        .reg_write(reg_write),
        .mem_read(mem_read),
        .mem_write(mem_write),
        .alu_src(alu_src),
        .wb_sel(wb_sel),
        .pc_src(pc_src),
        .is_lbu(is_lbu),
        .is_sb(is_sb),
        .illegal_instr(illegal_instr)
    );

    // Register file
    regfile u_regfile (
        .clk(clk),
        .rst_n(rst_n),
        .we(reg_write && !illegal_instr),
        .raddr1(rs1[3:0]),  // Only use lower 4 bits for x0-x15
        .raddr2(rs2[3:0]),
        .waddr(rd[3:0]),
        .wdata(wb_data),
        .rdata1(rs1_data),
        .rdata2(rs2_data)
    );

    // ALU input selection
    assign alu_a = rs1_data;
    assign alu_b = alu_src ? imm_i : rs2_data;

    // ALU
    alu u_alu (
        .a(alu_a),
        .b(alu_b),
        .result(alu_result)
    );

    // Data memory address
    assign dmem_addr = alu_result;

    // Data memory write data
    assign dmem_wdata = rs2_data;

    // Data memory write strobe
    // For sw: all 4 bytes (1111)
    // For sb: one-hot based on address[1:0]
    always_comb begin
        if (is_sb) begin
            case (dmem_addr[1:0])
                2'b00: dmem_wstrb = 4'b0001;
                2'b01: dmem_wstrb = 4'b0010;
                2'b10: dmem_wstrb = 4'b0100;
                2'b11: dmem_wstrb = 4'b1000;
                default: dmem_wstrb = 4'b0000;
            endcase
        end else begin
            dmem_wstrb = 4'b1111;  // sw or no write
        end
    end

    // Memory read data processing
    // For lw: use full word
    // For lbu: select byte and zero-extend
    assign byte_offset = dmem_addr[1:0];
    always_comb begin
        if (is_lbu) begin
            case (byte_offset)
                2'b00: mem_rdata_processed = {24'h0, dmem_rdata[7:0]};
                2'b01: mem_rdata_processed = {24'h0, dmem_rdata[15:8]};
                2'b10: mem_rdata_processed = {24'h0, dmem_rdata[23:16]};
                2'b11: mem_rdata_processed = {24'h0, dmem_rdata[31:24]};
                default: mem_rdata_processed = 32'h0;
            endcase
        end else begin
            mem_rdata_processed = dmem_rdata;  // lw
        end
    end

    // Writeback data selection
    always_comb begin
        case (wb_sel)
            2'b00: wb_data = alu_result;        // ALU result
            2'b01: wb_data = mem_rdata_processed; // Memory read
            2'b10: wb_data = pc_plus4;          // PC+4 (jalr)
            2'b11: wb_data = imm_u;             // U-type immediate (lui)
            default: wb_data = 32'h0;
        endcase
    end

    // PC next logic
    // For jalr: PC = (rs1 + imm) & ~1
    // Otherwise: PC = PC + 4
    logic [31:0] jalr_target;
    assign jalr_target = (rs1_data + imm_i) & 32'hFFFFFFFE;  // Clear LSB

    always_comb begin
        if (illegal_instr) begin
            pc_next = pc_plus4;  // NOP, just increment PC
        end else if (pc_src) begin
            pc_next = jalr_target;  // jalr
        end else begin
            pc_next = pc_plus4;  // Sequential
        end
    end

endmodule

