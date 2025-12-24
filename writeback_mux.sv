module writeback_mux(
    input logic [31:0] alu_result,
    input logic [31:0] mem_rdata,
    input logic [31:0] pc_plus4,
    input logic [31:0] imm_u,
    input logic [1:0] wb_sel,

    output logic [31:0] wb_data
);

    always_comb begin
        case (wb_sel)
            2'b00: wb_data = alu_result;
            2'b01: wb_data = mem_rdata;
            2'b10: wb_data = pc_plus4;
            2'b11: wb_data = imm_u;
            default: wb_data = 32'b0;
        endcase
    end
endmodule