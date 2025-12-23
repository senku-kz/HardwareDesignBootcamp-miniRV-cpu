module instruction_memory(
    input logic [31:0] address,
    output logic [31:0] instruction
);

    // Simple instruction memory with 16 instructions (4-bit address)
    logic [31:0] memory [0:15];

    // Initialize the instruction memory with example program:
    // Program: Load immediates, add them, and loop
    initial begin
        // Load data from "mem_data.hex" into the entire array
        $readmemh("logisim-bin/test.hex", memory);
    end

    // Read instruction from memory based on address
    always_comb begin
        instruction = memory[address];
    end

endmodule
