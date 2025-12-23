module program_counter(
    input logic clk,
    input logic [6:0] opcode,
    input logic [31:0] set_value,
    input logic reset,
    
    output logic [31:0] pc_out
);

always_ff @(posedge clk) begin
    if (reset) begin
        pc_out <= 32'h0;  // Reset PC to 0
    end else begin
        case (opcode)
            7'b1100111: pc_out <= set_value;  // JALR: PC = (rs1 + imm) & ~1
            
            // All other instructions: sequential execution
            // 7'b0110011: pc_out <= pc_out + 32'h4;  // ADD (R-type)
            // 7'b0010011: pc_out <= pc_out + 32'h4;  // ADDI (I-type)
            // 7'b0110111: pc_out <= pc_out + 32'h4;  // LUI (U-type)
            // 7'b0000011: pc_out <= pc_out + 32'h4;  // LW, LBU (I-type loads)
            // 7'b0100011: pc_out <= pc_out + 32'h4;  // SW, SB (S-type stores)
            
            default:    pc_out <= pc_out + 32'h4;  // Illegal instructions: NOP (increment PC)
        endcase
    end
end

endmodule
