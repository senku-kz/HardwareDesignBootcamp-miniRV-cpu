// miniRV.sv
// Top-level miniRV CPU module (RISC-V RV32E subset)

module miniRV (
    input  logic clk,
    input  logic reset,

    output logic [31:0] pc_,
    output logic [31:0] registers [0:15]
);

    // ========== Memory Interfaces ==========
    // Instruction memory interface (internal)
    logic [31:0] imem_addr;
    logic [31:0] imem_rdata;
    
    // Data memory interface (internal)
    logic [31:0] dmem_addr;
    logic [31:0] dmem_wdata;
    logic [3:0]  dmem_wstrb;
    logic [31:0] dmem_rdata;

    // ========== Internal Signals ==========
    // PC and instruction
    logic [31:0] pc, pc_next, pc_plus4;
    logic [31:0] instruction;
    
    // Decoded instruction fields
    logic [6:0]  opcode;
    logic [4:0]  rd, rs1, rs2;
    logic [2:0]  funct3;
    logic [6:0]  funct7;
    logic [31:0] imm_i, imm_s, imm_u;
    
    // Register file signals
    logic        reg_write;
    logic [31:0] rs1_data, rs2_data;
    logic [31:0] wb_data;
    
    // ALU signals
    logic [31:0] alu_a, alu_b, alu_result;
    
    // Control signals
    logic        alu_src;      // 0: rs2, 1: imm_i
    logic [1:0]  wb_sel;       // 00: ALU, 01: mem, 10: PC+4, 11: imm_u
    logic        pc_src;       // 0: PC+4, 1: jalr_target
    logic        is_lbu;       // is lbu instruction
    logic        is_sb;        // is sb instruction
    logic        mem_read;     // memory read enable
    logic        mem_write;    // memory write enable
    logic        illegal_reg;  // illegal register (x16-x31)
    logic        illegal_instr;// illegal instruction
    
    // Memory read data processing
    logic [31:0] mem_rdata_processed;
    logic [1:0]  byte_offset;
    
    // JALR target
    logic [31:0] jalr_target;
    
    // ========== PC Register ==========
    always_ff @(posedge clk or negedge reset) begin
        if (!reset) begin
            pc <= 32'h0;
        end else begin
            pc <= pc_next;
        end
    end
    
    assign pc_plus4 = pc + 32'h4;
    assign imem_addr = pc;
    
    // ========== Component Instantiation ==========
    
    // 1. Instruction Memory (ROM)
    instruction_fetch u_instruction_fetch (
        .address(imem_addr),
        .instruction(imem_rdata)
    );
    assign instruction = imem_rdata;
    
    // 2. Control Unit (Instruction Decoder)
    logic [1:0] wb_sel_cu;  // wb_sel from control_unit (not used, we generate our own)
    control_unit u_control_unit (
        .instruction(instruction),
        .opcode(opcode),
        .register_destination(rd),
        .register_source_1(rs1),
        .register_source_2(rs2),
        .function_3(funct3),
        .function_7(funct7),
        .immediate_7bit(),
        .immediate_12bit(),
        .immediate_20bit(),
        .immediate_32bit(),  // Not used, we use immediate_generator
        .wb_sel(wb_sel_cu)   // Not used, we generate our own
    );
    
    // 3. Immediate Generator
    immediate_generator u_immediate_generator (
        .instruction(instruction),
        .imm_i(imm_i),
        .imm_s(imm_s),
        .imm_u(imm_u)
    );
    
    // ========== Control Logic ==========
    // Check for illegal registers (x16-x31)
    assign illegal_reg = (rd[4] | rs1[4] | rs2[4]);
    
    // Control signals generation
    always_comb begin
        // Default values
        reg_write = 1'b0;
        alu_src = 1'b0;
        pc_src = 1'b0;
        mem_read = 1'b0;
        mem_write = 1'b0;
        is_lbu = 1'b0;
        is_sb = 1'b0;
        illegal_instr = 1'b0;
        wb_sel = 2'b00;
        
        case (opcode)
            // R-Type: ADD (0110011)
            7'b0110011: begin
                if (funct3 == 3'b000 && funct7 == 7'b0000000) begin
                    reg_write = 1'b1;
                    alu_src = 1'b0;  // Use rs2
                    wb_sel = 2'b00;  // ALU result
                end else begin
                    illegal_instr = 1'b1;
                end
            end
            
            // I-Type: ADDI, LW, LBU, JALR
            7'b0010011: begin
                // ADDI
                if (funct3 == 3'b000) begin
                    reg_write = 1'b1;
                    alu_src = 1'b1;  // Use imm_i
                    wb_sel = 2'b00;  // ALU result
                end else begin
                    illegal_instr = 1'b1;
                end
            end
            
            7'b0000011: begin
                // Load instructions
                mem_read = 1'b1;
                reg_write = 1'b1;
                alu_src = 1'b1;  // Use imm_i for address
                wb_sel = 2'b01;  // Memory data
                
                if (funct3 == 3'b010) begin
                    // LW
                    is_lbu = 1'b0;
                end else if (funct3 == 3'b100) begin
                    // LBU
                    is_lbu = 1'b1;
                end else begin
                    illegal_instr = 1'b1;
                end
            end
            
            7'b1100111: begin
                // JALR
                if (funct3 == 3'b000) begin
                    reg_write = 1'b1;
                    alu_src = 1'b1;  // Use imm_i
                    pc_src = 1'b1;   // Jump
                    wb_sel = 2'b10;  // PC+4
                end else begin
                    illegal_instr = 1'b1;
                end
            end
            
            // S-Type: SW, SB
            7'b0100011: begin
                mem_write = 1'b1;
                alu_src = 1'b1;  // Use imm_s for address
                
                if (funct3 == 3'b010) begin
                    // SW
                    is_sb = 1'b0;
                end else if (funct3 == 3'b000) begin
                    // SB
                    is_sb = 1'b1;
                end else begin
                    illegal_instr = 1'b1;
                end
            end
            
            // U-Type: LUI
            7'b0110111: begin
                reg_write = 1'b1;
                wb_sel = 2'b11;  // imm_u
            end
            
            default: begin
                illegal_instr = 1'b1;
            end
        endcase
        
        // Override if illegal register
        if (illegal_reg) begin
            illegal_instr = 1'b1;
            reg_write = 1'b0;
        end
    end
    
    // ========== Register File ==========
    // register_file now accepts 5-bit addresses (handles conversion internally)
    register_file u_register_file (
        .clk(clk),
        .reset(reset),
        .we(reg_write && !illegal_instr),
        .raddr1(rs1),
        .raddr2(rs2),
        .waddr(rd),
        .wdata(wb_data),
        .rdata1(rs1_data),
        .rdata2(rs2_data)
    );
    
    // ========== ALU ==========
    assign alu_a = rs1_data;
    
    // ALU B selection: stores use imm_s, others use imm_i or rs2
    always_comb begin
        if (mem_write) begin
            alu_b = imm_s;  // S-type uses imm_s
        end else begin
            alu_b = alu_src ? imm_i : rs2_data;
        end
    end
    
    alu u_alu (
        .operand_a(alu_a),
        .operand_b(alu_b),
        .function_3(funct3),
        .function_7(funct7),
        .result(alu_result)
    );
    
    // ========== Data Memory ==========
    assign dmem_addr = alu_result;
    assign dmem_wdata = rs2_data;
    
    // Write strobe generation
    always_comb begin
        if (is_sb && mem_write) begin
            // Byte store: one-hot based on address[1:0]
            case (dmem_addr[1:0])
                2'b00: dmem_wstrb = 4'b0001;
                2'b01: dmem_wstrb = 4'b0010;
                2'b10: dmem_wstrb = 4'b0100;
                2'b11: dmem_wstrb = 4'b1000;
                default: dmem_wstrb = 4'b0000;
            endcase
        end else if (mem_write) begin
            // Word store
            dmem_wstrb = 4'b1111;
        end else begin
            dmem_wstrb = 4'b0000;
        end
    end
    
    // Simple data memory model (internal)
    parameter DMEM_SIZE = 1024;
    logic [31:0] dmem [0:DMEM_SIZE-1];
    
    // Memory address calculation
    logic [31:0] dmem_word_addr;
    logic [31:0] old_word, new_word;
    assign dmem_word_addr = dmem_addr >> 2;
    
    // Memory write
    always_ff @(posedge clk) begin
        if (mem_write && !illegal_instr) begin
            if (dmem_word_addr < DMEM_SIZE) begin
                old_word = dmem[dmem_word_addr];
                new_word = old_word;
                
                if (dmem_wstrb[0]) new_word[7:0]   = dmem_wdata[7:0];
                if (dmem_wstrb[1]) new_word[15:8]  = dmem_wdata[15:8];
                if (dmem_wstrb[2]) new_word[23:16] = dmem_wdata[23:16];
                if (dmem_wstrb[3]) new_word[31:24] = dmem_wdata[31:24];
                
                dmem[dmem_word_addr] <= new_word;
            end
        end
    end
    
    // Memory read
    always_comb begin
        if (dmem_word_addr < DMEM_SIZE) begin
            dmem_rdata = dmem[dmem_word_addr];
        end else begin
            dmem_rdata = 32'h0;
        end
    end
    
    // Memory read data processing
    assign byte_offset = dmem_addr[1:0];
    always_comb begin
        if (is_lbu && mem_read) begin
            // Byte load: select byte and zero-extend
            case (byte_offset)
                2'b00: mem_rdata_processed = {24'h0, dmem_rdata[7:0]};
                2'b01: mem_rdata_processed = {24'h0, dmem_rdata[15:8]};
                2'b10: mem_rdata_processed = {24'h0, dmem_rdata[23:16]};
                2'b11: mem_rdata_processed = {24'h0, dmem_rdata[31:24]};
                default: mem_rdata_processed = 32'h0;
            endcase
        end else begin
            mem_rdata_processed = dmem_rdata;  // lw or no load
        end
    end
    
    // ========== Writeback Mux ==========
    writeback_mux u_writeback_mux (
        .alu_result(alu_result),
        .mem_rdata(mem_rdata_processed),
        .pc_plus4(pc_plus4),
        .imm_u(imm_u),
        .wb_sel(wb_sel),
        .wb_data(wb_data)
    );
    
    // ========== PC Next Logic ==========
    // For jalr: PC = (rs1 + imm_i) & ~1
    assign jalr_target = (rs1_data + imm_i) & 32'hFFFFFFFE;
    
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
