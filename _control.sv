// control.sv
// Control logic for miniRV CPU

module control (
    input  logic [6:0] opcode,
    input  logic [2:0] funct3,
    input  logic       illegal_reg,
    output logic       reg_write,     // write to register file
    output logic       mem_read,       // memory read enable
    output logic       mem_write,      // memory write enable
    output logic       alu_src,        // 0: rs2, 1: immediate
    output logic [1:0] wb_sel,         // writeback select: 00=ALU, 01=mem, 10=PC+4, 11=lui
    output logic       pc_src,         // 0: PC+4, 1: jalr
    output logic       is_lbu,         // load byte unsigned
    output logic       is_sb,          // store byte
    output logic       illegal_instr   // illegal instruction
);

    // Instruction type detection
    logic is_add, is_addi, is_lui, is_lw, is_lbu_internal, is_sw, is_sb_internal, is_jalr;

    // Decode instructions based on opcode and funct3
    assign is_add  = (opcode == 7'b0110011) && (funct3 == 3'b000);  // ADD
    assign is_addi = (opcode == 7'b0010011) && (funct3 == 3'b000);  // ADDI
    assign is_lui  = (opcode == 7'b0110111);                        // LUI
    assign is_lw   = (opcode == 7'b0000011) && (funct3 == 3'b010);  // LW
    assign is_lbu_internal = (opcode == 7'b0000011) && (funct3 == 3'b100);   // LBU
    assign is_sw   = (opcode == 7'b0100011) && (funct3 == 3'b010);  // SW
    assign is_sb_internal = (opcode == 7'b0100011) && (funct3 == 3'b000);   // SB
    assign is_jalr = (opcode == 7'b1100111) && (funct3 == 3'b000);  // JALR

    // Illegal instruction if not one of the 8 supported instructions
    assign illegal_instr = !(is_add || is_addi || is_lui || is_lw || is_lbu_internal || is_sw || is_sb_internal || is_jalr) || illegal_reg;

    // Control signals
    assign reg_write = !illegal_instr && (is_add || is_addi || is_lui || is_lw || is_lbu_internal || is_jalr);
    assign mem_read  = !illegal_instr && (is_lw || is_lbu_internal);
    assign mem_write = !illegal_instr && (is_sw || is_sb_internal);
    assign alu_src   = !illegal_instr && (is_addi || is_lw || is_lbu_internal || is_sw || is_sb_internal || is_jalr);
    assign is_lbu    = !illegal_instr && is_lbu_internal;
    assign is_sb     = !illegal_instr && is_sb_internal;

    // Writeback select:
    // 00: ALU result
    // 01: Memory read data
    // 10: PC + 4 (for jalr)
    // 11: U-type immediate (for lui)
    assign wb_sel = illegal_instr ? 2'b00 :
                    is_lui        ? 2'b11 :
                    is_jalr       ? 2'b10 :
                    (is_lw || is_lbu_internal) ? 2'b01 : 2'b00;

    // PC source: 0 = PC+4, 1 = jalr
    assign pc_src = !illegal_instr && is_jalr;

endmodule

