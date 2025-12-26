#ifndef GOLDEN_MODEL_CPU_H
#define GOLDEN_MODEL_CPU_H

#include <cstdint>
#include <cstddef>
#include <string>

// Global cycle limit
extern int CYCLE_LIMIT;

class GoldenModelCPU {
private:
    static constexpr size_t REGISTER_LIMIT = 16;

public:
    // Public attributes as requested
    bool clock;
    bool reset;
    uint32_t pc;              // Program Counter
    uint32_t registers[REGISTER_LIMIT];        // 16 registers (x0-x15)
    
    // Instruction memory and data memory
    static constexpr size_t IMEM_SIZE = 128 * 1024; 
    static constexpr size_t DMEM_SIZE = 128 * 1024;
    
    uint32_t imem[IMEM_SIZE];  // Instruction memory (word-addressable)
    uint32_t dmem[DMEM_SIZE];  // Data memory (word-addressable, 32-bit data width)
    
    // Constructor
    GoldenModelCPU();
    
    // Load instructions from hex file (Logisim format)
    bool loadHexFile(const std::string& filename);
    
    // Reset CPU
    void resetCPU();
    
    // Execute one instruction (single cycle)
    bool executeInstruction();
    
    // Clock cycle: execute one instruction
    void clockCycle();
    
    // Run CPU for N cycles
    void runCycles(int cycles);
    
    // Print CPU state
    void printState();
    
    // Get instruction at address
    uint32_t getInstruction(uint32_t addr);
    
    // Read data from memory
    uint32_t readMem(uint32_t addr);
};

#endif // GOLDEN_MODEL_CPU_H
