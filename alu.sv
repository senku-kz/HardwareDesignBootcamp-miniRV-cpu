module alu(
    input logic [31:0] operand_a,
    input logic [31:0] operand_b,
    input logic [2:0] function_3, // 000 = add, 000 = sub, 111 = andi, 110 = or1
    input logic [6:0] function_7, // 0000000 = add, 0100000 = sub, 0000000 = and, 0000000 = or

    output logic [31:0] result
    // output logic zero_flag
);

    always_comb begin
        case (function_3)
            3'b000: begin
                if ( function_7 == 7'b0000000 ) begin
                    result = operand_a + operand_b; // add
                end else if ( function_7 == 7'b0100000 ) begin
                    result = operand_a - operand_b; // sub
                end
            end
            3'b111: begin
                result = operand_a & operand_b; // andi
            end
            3'b110: begin
                result = operand_a | operand_b; // ori
            end
            default: result = 32'b00000000000000000000000000000000; // default case
        endcase
    end
endmodule
