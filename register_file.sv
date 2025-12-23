// regfile.sv
// 16 x 32-bit register file with 2 read ports, 1 write port
// x0 is hardwired to zero

module register_file (
    input  logic        clk,
    input  logic        reset,
    input  logic        we,           // write enable
    input  logic [3:0]  raddr1,       // read address 1 (rs1)
    input  logic [3:0]  raddr2,       // read address 2 (rs2)
    input  logic [3:0]  waddr,        // write address (rd)
    input  logic [31:0] wdata,        // write data
    
    output logic [31:0] rdata1,       // read data 1
    output logic [31:0] rdata2        // read data 2
);

    // 16 x 32-bit registers (x0-x15)
    logic [31:0] regs [15:0];

    // Write port: writes are ignored for x0
    always_ff @(posedge clk or negedge reset) begin
        if (reset) begin
            for (int i = 0; i < 16; i++) begin
                regs[i] <= 32'h0;
            end
        end else if (we && waddr != 4'h0) begin
            regs[waddr] <= wdata;
        end
    end

    // Read port 1: x0 always returns 0
    assign rdata1 = (raddr1 == 4'h0) ? 32'h0 : regs[raddr1];

    // Read port 2: x0 always returns 0
    assign rdata2 = (raddr2 == 4'h0) ? 32'h0 : regs[raddr2];

endmodule
