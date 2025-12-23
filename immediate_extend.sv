module immediate_extend(
    input logic [11:0] imm_in,

    output logic [31:0] imm_out
);
    always_comb begin
        imm_out = {20'b00000000000000000000, imm_in}; // zero-extend
    end
endmodule
