module program_counter(
    input logic clk,
    input logic [6:0] opcode,
    input logic [31:0] set_value,
    input logic reset,
    output logic [31:0] pc_out
);

always_ff @(posedge clk) begin

    if (reset) begin
        pc_out <= 32'b0000; // Reset PC to 0
    end else if (opcode == 7'b1100111) begin
        pc_out <= set_value; // Branch instruction sets PC to address
    end else begin
        pc_out <= pc_out + 32'b00000000000000000000000000000100; // Increment PC by 4
    end

end

endmodule
