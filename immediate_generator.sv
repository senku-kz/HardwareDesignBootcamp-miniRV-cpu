module immediate_generator (
  input  logic [31:0] instruction,
  output logic [31:0] imm_i,
  output logic [31:0] imm_s,
  output logic [31:0] imm_u
);
  // I-type imm[11:0] sign-extended
  assign imm_i = {{20{instruction[31]}}, instruction[31:20]};

  // S-type imm[11:5|4:0] sign-extended
  assign imm_s = {{20{instruction[31]}}, instruction[31:25], instruction[11:7]};

  // U-type imm[31:12] << 12
  assign imm_u = {instruction[31],instruction[30:12], 12'b0};
endmodule
