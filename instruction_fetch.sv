module instruction_fetch(
    input logic [31:0] address,

    output logic [31:0] instruction
);

    parameter MEMORY_SIZE = 32;
    logic [31:0] memory [0:MEMORY_SIZE-1];

    // Initialize the instruction memory with example program:
    // Program: Load immediates, add them, and loop
    initial begin
        // Load data from "test.hex" into the entire array
        $readmemh("logisim-bin/test-pc4.hex", memory);
    end

    // logic [31:0] masked_address = 32'b1111_1111_1111_1111_1111_1111_1111_1100;
    logic [31:0] word_index = address >> 2;

    always_comb begin
        // Convert byte address to word index: address[6:2] for 32 words
        // This handles addresses 0, 4, 8, 12, ... up to 124 (31 words)
        instruction = memory[word_index];
    end

endmodule
