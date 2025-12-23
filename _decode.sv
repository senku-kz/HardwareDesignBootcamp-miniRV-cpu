// decode.sv
// Instruction decoder and immediate generator

module decode (
    input  logic [31:0] instr,
    output logic [6:0]  opcode,
    output logic [4:0]  rd,
    output logic [2:0]  funct3,
    output logic [4:0]  rs1,
    output logic [4:0]  rs2,
    output logic [6:0]  funct7,
    output logic [31:0] imm_i,        // I-type immediate (sign-extended)
    output logic [31:0] imm_s,        // S-type immediate (sign-extended)
    output logic [31:0] imm_u,        // U-type immediate
    output logic        illegal_reg   // true if rd/rs1/rs2 >= 16
);

    // Extract instruction fields
    assign opcode = instr[6:0];
    assign rd     = instr[11:7];
    assign funct3 = instr[14:12];
    assign rs1    = instr[19:15];
    assign rs2    = instr[24:20];
    assign funct7 = instr[31:25];

    // I-type immediate: sign-extended 12-bit
    assign imm_i = {{20{instr[31]}}, instr[31:20]};

    // S-type immediate: sign-extended 12-bit
    assign imm_s = {{20{instr[31]}}, instr[31:25], instr[11:7]};

    // U-type immediate: imm[31:12] << 12
    assign imm_u = {instr[31:12], 12'h0};

    // Check for illegal register indices (x16-x31)
    assign illegal_reg = (rd >= 5'd16) || (rs1 >= 5'd16) || (rs2 >= 5'd16);

endmodule

