// miniRV.sv
// Top-level miniRV CPU module

module miniRV (
    input logic clk,
    input logic reset
);

    // Internal signals
    logic [31:0] pc = 32'h0000, pc_step = 32'h0004;
    logic [31:0] imem_addr_32bit, imem_rdata, imm_32bit;
    logic [11:0] imm_12bit;

    // PC register
    always_ff @(posedge clk or negedge reset) begin
        if (reset == 1'b1) pc <= 32'h0000;
        else pc <= pc + pc_step;
    end

    // PC update logic
    assign imem_addr = pc;


    // ========== Component Instantiation ==========
    
    // 1. Program Counter
    program_counter u_program_counter (
        .clk(clk),
        .opcode(opcode),
        .set_value(set_value),
        .reset(reset),
        .pc_out(imem_addr_32bit)
    );

    // 2. Instruction Memory (ROM)
    instruction_memory u_instruction_memory (
        .address(imem_addr_32bit),
        .instruction(imem_rdata)
    );

    // 3. Control Unit (Instruction Decoder)
    control_unit u_control_unit (
        .instruction(imem_rdata),
        .opcode(opcode),
        .rd(rd),
        .rs1(raddr1),
        .rs2(raddr2),
        .imm(imm_12bit),
        .funct3(funct3),
        .funct7(funct7)
    );

    // 4. Immediate Extend
    immediate_extend u_immediate_extend (
        .imm_in(imm_12bit),
        .imm_out(imm_32bit)
    );    

    // 5. Register File
    register_file u_register_file (
        .clk(clk),
        .reset(reset),
        .we(we),
        .raddr1(raddr1),
        .raddr2(raddr2),
        .waddr(waddr),
        .wdata(wdata),
        .rdata1(rdata1),
        .rdata2(rdata2)
    );

    // 6. ALU
    arithmetic_logic_unit u_arithmetic_logic_unit (
        .a(rdata1),
        .b(rdata2),
        .alu_op(alu_op),
        .result(alu_result_32bit)
    );

    // ========== Control Logic ==========
    always_comb begin
        case (opcode)
            7'b0100011: begin // sw
                dmem_wstrb = 4'b1111;
            end
            7'b0100011: begin // sb
                dmem_wstrb = 4'b1111;
            end
        endcase
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

