module instruction_fetch(
    input logic [31:0] address,

    output logic [31:0] instruction
);

    // 24-bit byte address = 2^24 bytes = 16,777,216 bytes
    // For 32-bit words: 2^24 / 4 = 2^22 words
    parameter MEMORY_SIZE = 1 << 22;  // 2^22 = 4,194,304 words (24-bit byte addressable)
    logic [31:0] memory [0:MEMORY_SIZE-1];

    parameter INSTRUCTION_MEMORY_FILE = "logisim-bin/sum.memh";

    // Initialize the instruction memory with example program:
    // Program: Load immediates, add them, and loop
    initial begin       
        // Load data from file into the array (limited to MEMORY_SIZE)
        $readmemh(INSTRUCTION_MEMORY_FILE, memory, 0, MEMORY_SIZE-1);
    end

    // logic [31:0] masked_address = 32'b1111_1111_1111_1111_1111_1111_1111_1100;
    logic [31:0] word_index = address >> 2;

    always_comb begin
        // Convert byte address to word index: address[6:2] for 32 words
        // This handles addresses 0, 4, 8, 12, ... up to 124 (31 words)
        instruction = memory[word_index];
    end

endmodule
