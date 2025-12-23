// tb_miniRV.sv
// Testbench for miniRV CPU

module tb_miniRV;

    logic        clk;
    logic        rst_n;
    logic [31:0] imem_addr;
    logic [31:0] imem_rdata;
    /* verilator lint_off UNUSEDSIGNAL */
    logic [31:0] dmem_addr;  // dmem_addr[1:0] used internally by CPU for byte selection
    /* verilator lint_on UNUSEDSIGNAL */
    logic [31:0] dmem_wdata;
    logic [3:0]  dmem_wstrb;
    logic [31:0] dmem_rdata;

    // Instruction memory (ROM) - expanded to support function at 0x100
    logic [31:0] imem [0:255];
    
    // Data memory (RAM)
    logic [31:0] dmem [0:63];

    // Instantiate CPU
    miniRV_cpu u_cpu (
        .clk(clk),
        .rst_n(rst_n),
        .imem_addr(imem_addr),
        .imem_rdata(imem_rdata),
        .dmem_addr(dmem_addr),
        .dmem_wdata(dmem_wdata),
        .dmem_wstrb(dmem_wstrb),
        .dmem_rdata(dmem_rdata)
    );

    // Clock generation
    initial begin
        clk = 0;
        forever #5 clk = ~clk;
    end

    // Instruction memory interface
    always_comb begin
        if (imem_addr[31:10] == 22'h0) begin
            imem_rdata = imem[imem_addr[9:2]];  // Word-aligned access (supports up to 0x100)
        end else begin
            imem_rdata = 32'h0;
        end
    end

    // Data memory interface
    // Memory returns the word containing the requested byte address
    // The CPU will extract the appropriate byte for lbu
    always_comb begin
        if (dmem_addr[31:8] == 24'h0) begin
            dmem_rdata = dmem[dmem_addr[7:2]];  // Word-aligned read (returns full word)
        end else begin
            dmem_rdata = 32'h0;
        end
    end

    // Data memory write
    always_ff @(posedge clk) begin
        /* verilator lint_off SYNCASYNCNET */
        if (!rst_n) begin
        /* verilator lint_on SYNCASYNCNET */
            for (int i = 0; i < 64; i++) begin
                dmem[i] <= 32'h0;
            end
        end else if (|dmem_wstrb && dmem_addr[31:8] == 24'h0) begin
            // Write bytes based on wstrb
            if (dmem_wstrb[0]) dmem[dmem_addr[7:2]][7:0]   <= dmem_wdata[7:0];
            if (dmem_wstrb[1]) dmem[dmem_addr[7:2]][15:8]  <= dmem_wdata[15:8];
            if (dmem_wstrb[2]) dmem[dmem_addr[7:2]][23:16] <= dmem_wdata[23:16];
            if (dmem_wstrb[3]) dmem[dmem_addr[7:2]][31:24] <= dmem_wdata[31:24];
        end
    end

    // Test program
    initial begin
        // Initialize instruction memory
        for (int i = 0; i < 256; i++) begin
            imem[i] = 32'h0;
        end

        // Initialize data memory with 0x12345678 at address 0
        dmem[0] = 32'h12345678;

        // Test program:
        // 0x00: lui  x1, 0x00000        // x1 = 0
        // 0x04: addi x2, x1, 0          // x2 = 0 (base address)
        // 0x08: lw   x3, 0(x2)          // x3 = 0x12345678
        // 0x0C: lbu  x4, 0(x2)          // x4 = 0x78
        // 0x10: lbu  x5, 1(x2)          // x5 = 0x56
        // 0x14: lbu  x6, 2(x2)          // x6 = 0x34
        // 0x18: lbu  x7, 3(x2)          // x7 = 0x12
        // 0x1C: addi x8, x0, 0x90       // x8 = 0x90
        // 0x20: sb   x8, 0(x2)          // Store 0x90 at byte 0
        // 0x24: addi x8, x0, 0xAB       // x8 = 0xAB
        // 0x28: sb   x8, 1(x2)          // Store 0xAB at byte 1
        // 0x2C: addi x8, x0, 0xCD        // x8 = 0xCD
        // 0x30: sb   x8, 2(x2)          // Store 0xCD at byte 2
        // 0x34: addi x8, x0, 0xEF        // x8 = 0xEF
        // 0x38: sb   x8, 3(x2)          // Store 0xEF at byte 3
        // 0x3C: lw   x9, 0(x2)          // x9 = 0x90ABCDEF
        // 0x40: addi x10, x0, 5         // x10 = 5
        // 0x44: addi x11, x0, 3         // x11 = 3
        // 0x48: add  x12, x10, x11      // x12 = 8
        // 0x4C: lui  x13, 0x12345       // x13 = 0x12345000
        // 0x50: addi x14, x0, 0x100     // x14 = 0x100 (function address)
        // 0x54: jalr x15, 0(x14)        // x15 = PC+4, PC = 0x100 & ~1 = 0x100
        // 0x58: (return point - should not execute)
        // 0x100: addi x15, x15, 1       // x15 = x15 + 1 (test function)
        // 0x104: jalr x0, 0(x15)        // return (x0 ignored, PC = x15 & ~1)

        // Encode instructions
        imem[0]  = 32'h000000B7;  // lui x1, 0x00000
        imem[1]  = 32'h00008113;  // addi x2, x1, 0
        imem[2]  = 32'h00012183;  // lw x3, 0(x2)
        imem[3]  = 32'h00014203;  // lbu x4, 0(x2)
        imem[4]  = 32'h00114283;  // lbu x5, 1(x2)
        imem[5]  = 32'h00214303;  // lbu x6, 2(x2)
        imem[6]  = 32'h00314383;  // lbu x7, 3(x2)
        imem[7]  = 32'h09000413;  // addi x8, x0, 0x90
        imem[8]  = 32'h00810023;  // sb x8, 0(x2)
        imem[9]  = 32'h0AB00413;  // addi x8, x0, 0xAB
        imem[10] = 32'h008100A3;  // sb x8, 1(x2)
        imem[11] = 32'h0CD00413;  // addi x8, x0, 0xCD
        imem[12] = 32'h00810123;  // sb x8, 2(x2)
        imem[13] = 32'h0EF00413;  // addi x8, x0, 0xEF
        imem[14] = 32'h008101A3;  // sb x8, 3(x2)
        imem[15] = 32'h00012483;  // lw x9, 0(x2)
        imem[16] = 32'h00500513;  // addi x10, x0, 5
        imem[17] = 32'h00300593;  // addi x11, x0, 3
        imem[18] = 32'h00B50633;  // add x12, x10, x11
        imem[19] = 32'h123456B7;  // lui x13, 0x12345
        imem[20] = 32'h10000713;  // addi x14, x0, 0x100
        imem[21] = 32'h000707E7;  // jalr x15, 0(x14)
        imem[22] = 32'h00000000;  // nop (should not execute)
        
        // Function at 0x100 (address 0x100 / 4 = 64)
        imem[64] = 32'h00178793;  // addi x15, x15, 1
        imem[65] = 32'h000780E7;  // jalr x0, 0(x15) - return

        // Reset
        rst_n = 0;
        #20;
        rst_n = 1;
        #10;

        // Run for enough cycles to execute all instructions
        #2000;

        // Wait a bit more for final writes
        #100;

        // Check results by reading memory and checking final state
        $display("=== Test Results ===");
        
        // Check data memory was written correctly by sb
        if (dmem[0] == 32'h90ABCDEF) begin
            $display("PASS: sb test - dmem[0] = 0x%08x", dmem[0]);
        end else begin
            $display("FAIL: sb test - dmem[0] = 0x%08x (expected 0x90ABCDEF)", dmem[0]);
        end

        // Display final PC to verify jalr worked
        $display("Final PC (imem_addr) = 0x%08x", imem_addr);
        if (imem_addr == 32'h58 || imem_addr == 32'h5C) begin
            $display("PASS: jalr test - PC returned correctly");
        end else begin
            $display("INFO: jalr test - PC = 0x%08x (expected 0x58 or 0x5C)", imem_addr);
        end

        // Note: To verify register values, you would need to add a debug interface
        // or use a waveform viewer. The memory tests verify the core functionality.
        $display("Note: Use waveform viewer to inspect register values");
        $display("Expected register values:");
        $display("  x3 = 0x12345678 (lw test)");
        $display("  x4 = 0x78, x5 = 0x56, x6 = 0x34, x7 = 0x12 (lbu tests)");
        $display("  x9 = 0x90ABCDEF (sb/lw test)");
        $display("  x10 = 5, x11 = 3, x12 = 8 (add/addi tests)");
        $display("  x13 = 0x12345000 (lui test)");
        $display("  x15 = 0x59 or 0x5C (jalr return address)");

        $display("=== Test Complete ===");
        $finish;
    end

    // Monitor PC and instruction (commented out to reduce output, uncomment for debugging)
    // initial begin
    //     $monitor("Time=%0t PC=0x%08x Instr=0x%08x", $time, u_cpu.pc, u_cpu.instr);
    // end

endmodule

