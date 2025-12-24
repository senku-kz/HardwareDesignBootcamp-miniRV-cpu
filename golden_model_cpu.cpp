#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdint>
#include <cstring>

// Golden Model CPU for miniRV (RISC-V RV32E subset)
// This is a functional C++ model that executes instructions from hex files

class GoldenModelCPU {
public:
    // Public attributes as requested
    bool clock;
    bool reset;
    uint32_t pc;              // Program Counter
    uint32_t regs[16];        // 16 registers (x0-x15)
    
    // Instruction memory and data memory
    static constexpr size_t IMEM_SIZE = 1024 * 1024;  // 1M words
    static constexpr size_t DMEM_SIZE = 1024 * 1024;  // 1M words
    
    uint32_t imem[IMEM_SIZE];  // Instruction memory
    uint8_t  dmem[DMEM_SIZE];  // Data memory (byte-addressable)
    
    // Constructor
    GoldenModelCPU() : clock(false), reset(false), pc(0) {
        // Initialize registers (x0 is always 0, others can be 0 initially)
        for (int i = 0; i < 16; i++) {
            regs[i] = 0;
        }
        
        // Initialize memories
        memset(imem, 0, sizeof(imem));
        memset(dmem, 0, sizeof(dmem));
    }
    
    // Load instructions from hex file (Logisim format)
    bool loadHexFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << std::endl;
            return false;
        }
        
        std::string line;
        bool first_line = true;
        
        while (std::getline(file, line)) {
            // Skip empty lines
            if (line.empty()) continue;
            
            // Skip version header line
            if (first_line && line.find("v3.0") != std::string::npos) {
                first_line = false;
                continue;
            }
            first_line = false;
            
            // Parse line: address: instruction1 instruction2 ...
            size_t colon_pos = line.find(':');
            if (colon_pos == std::string::npos) continue;
            
            std::string addr_str = line.substr(0, colon_pos);
            std::string instr_str = line.substr(colon_pos + 1);
            
            // Parse base address
            uint32_t base_addr = std::stoul(addr_str, nullptr, 16);
            base_addr = base_addr >> 2;  // Convert byte address to word index
            
            // Parse instructions
            std::istringstream iss(instr_str);
            std::string instr_hex;
            uint32_t offset = 0;
            
            while (iss >> instr_hex) {
                if (instr_hex.empty()) continue;
                
                uint32_t instr = std::stoul(instr_hex, nullptr, 16);
                uint32_t word_addr = base_addr + offset;
                
                if (word_addr < IMEM_SIZE) {
                    imem[word_addr] = instr;
                } else {
                    std::cerr << "Warning: Instruction address " << word_addr 
                              << " exceeds memory size" << std::endl;
                }
                
                offset++;
            }
        }
        
        file.close();
        return true;
    }
    
    // Reset CPU
    void resetCPU() {
        reset = true;
        pc = 0;
        for (int i = 0; i < 16; i++) {
            regs[i] = 0;
        }
        reset = false;
    }
    
    // Execute one instruction (single cycle)
    bool executeInstruction() {
        // Fetch instruction
        uint32_t word_index = pc >> 2;
        if (word_index >= IMEM_SIZE) {
            std::cerr << "Error: PC out of bounds: 0x" << std::hex << pc << std::dec << std::endl;
            return false;
        }
        
        uint32_t instr = imem[word_index];
        
        // Decode instruction
        uint8_t opcode = instr & 0x7F;
        uint8_t rd = (instr >> 7) & 0x1F;
        uint8_t funct3 = (instr >> 12) & 0x7;
        uint8_t rs1 = (instr >> 15) & 0x1F;
        uint8_t rs2 = (instr >> 20) & 0x1F;
        uint8_t funct7 = (instr >> 25) & 0x7F;
        
        // Calculate PC+4
        uint32_t pc_plus4 = pc + 4;
        
        // Initialize next PC to PC+4 (sequential execution)
        uint32_t next_pc = pc_plus4;
        
        // Extract immediate values
        int32_t imm_i = (int32_t)(instr) >> 20;  // Sign-extended 12-bit immediate
        int32_t imm_s = ((int32_t)(instr & 0xFE000000) >> 20) | ((instr >> 7) & 0x1F);
        if (imm_s & 0x800) imm_s |= 0xFFFFF800;  // Sign extend
        uint32_t imm_u = instr & 0xFFFFF000;  // Upper 20 bits, lower 12 bits are 0
        
        // Check for illegal registers (x16-x31)
        bool illegal_reg = (rd >= 16) || (rs1 >= 16) || (rs2 >= 16);
        
        // Execute instruction based on opcode
        switch (opcode) {
            case 0x33: {  // R-Type: ADD
                if (funct3 == 0x0 && funct7 == 0x0 && !illegal_reg) {
                    if (rd < 16 && rd != 0) {
                        regs[rd] = regs[rs1] + regs[rs2];
                    }
                }
                break;
            }
            
            case 0x13: {  // I-Type: ADDI
                if (funct3 == 0x0 && !illegal_reg) {
                    if (rd < 16 && rd != 0) {
                        regs[rd] = regs[rs1] + imm_i;
                    }
                }
                break;
            }
            
            case 0x37: {  // U-Type: LUI
                if (!illegal_reg && rd < 16 && rd != 0) {
                    regs[rd] = imm_u;
                }
                break;
            }
            
            case 0x03: {  // I-Type: Load instructions
                if (!illegal_reg && rd < 16) {
                    uint32_t addr = regs[rs1] + imm_i;
                    uint32_t word_addr = addr >> 2;
                    uint8_t byte_offset = addr & 0x3;
                    
                    if (word_addr < (DMEM_SIZE >> 2)) {
                        uint32_t mem_word = 0;
                        mem_word |= ((uint32_t)dmem[word_addr * 4 + 0]) << 0;
                        mem_word |= ((uint32_t)dmem[word_addr * 4 + 1]) << 8;
                        mem_word |= ((uint32_t)dmem[word_addr * 4 + 2]) << 16;
                        mem_word |= ((uint32_t)dmem[word_addr * 4 + 3]) << 24;
                        
                        if (funct3 == 0x2) {  // LW - Load word
                            if (rd != 0) {
                                regs[rd] = mem_word;
                            }
                        } else if (funct3 == 0x4) {  // LBU - Load byte unsigned
                            if (rd != 0) {
                                uint8_t byte_val = (mem_word >> (byte_offset * 8)) & 0xFF;
                                regs[rd] = (uint32_t)byte_val;  // Zero-extend
                            }
                        }
                    }
                }
                break;
            }
            
            case 0x23: {  // S-Type: Store instructions
                if (!illegal_reg) {
                    uint32_t addr = regs[rs1] + imm_s;
                    uint32_t word_addr = addr >> 2;
                    uint8_t byte_offset = addr & 0x3;
                    
                    if (word_addr < (DMEM_SIZE >> 2)) {
                        if (funct3 == 0x2) {  // SW - Store word
                            dmem[word_addr * 4 + 0] = (regs[rs2] >> 0) & 0xFF;
                            dmem[word_addr * 4 + 1] = (regs[rs2] >> 8) & 0xFF;
                            dmem[word_addr * 4 + 2] = (regs[rs2] >> 16) & 0xFF;
                            dmem[word_addr * 4 + 3] = (regs[rs2] >> 24) & 0xFF;
                        } else if (funct3 == 0x0) {  // SB - Store byte
                            dmem[word_addr * 4 + byte_offset] = regs[rs2] & 0xFF;
                        }
                    }
                }
                break;
            }
            
            case 0x67: {  // I-Type: JALR
                if (funct3 == 0x0 && !illegal_reg) {
                    if (rd < 16 && rd != 0) {
                        regs[rd] = pc_plus4;
                    }
                    uint32_t target = (regs[rs1] + imm_i) & 0xFFFFFFFE;  // Clear LSB
                    next_pc = target;
                }
                break;
            }
            
            default: {
                // Unknown instruction - treat as NOP (just increment PC)
                break;
            }
        }
        
        // Update PC
        pc = next_pc;
        
        // Ensure x0 is always zero
        regs[0] = 0;
        
        return true;
    }
    
    // Clock cycle: execute one instruction
    void clockCycle() {
        if (!reset) {
            clock = true;
            executeInstruction();
            clock = false;
        }
    }
    
    // Run CPU for N cycles
    void runCycles(int cycles) {
        for (int i = 0; i < cycles; i++) {
            clockCycle();
        }
    }
    
    // Print CPU state
    void printState() {
        std::cout << "CPU State:\n";
        std::cout << "  PC: 0x" << std::hex << std::setfill('0') << std::setw(8) << pc << std::dec << "\n";
        std::cout << "  Registers:\n";
        for (int i = 0; i < 16; i++) {
            std::cout << "    x" << i << ": 0x" << std::hex << std::setfill('0') << std::setw(8) 
                      << regs[i] << std::dec;
            if (i % 4 == 3) std::cout << "\n";
            else std::cout << "  ";
        }
        if (16 % 4 != 0) std::cout << "\n";
    }
    
    // Get instruction at address
    uint32_t getInstruction(uint32_t addr) {
        uint32_t word_index = addr >> 2;
        if (word_index < IMEM_SIZE) {
            return imem[word_index];
        }
        return 0;
    }
    
    // Read data from memory
    uint32_t readMem(uint32_t addr) {
        if (addr < DMEM_SIZE) {
            uint32_t word_addr = addr >> 2;
            uint32_t mem_word = 0;
            mem_word |= ((uint32_t)dmem[word_addr * 4 + 0]) << 0;
            mem_word |= ((uint32_t)dmem[word_addr * 4 + 1]) << 8;
            mem_word |= ((uint32_t)dmem[word_addr * 4 + 2]) << 16;
            mem_word |= ((uint32_t)dmem[word_addr * 4 + 3]) << 24;
            return mem_word;
        }
        return 0;
    }
};

// Example usage
int main(int argc, char** argv) {
    std::string hex_file = "logisim-bin/sum.hex";
    if (argc > 1) {
        hex_file = argv[1];
    }
    
    GoldenModelCPU cpu;
    
    // Load instructions
    std::cout << "Loading instructions from " << hex_file << "...\n";
    if (!cpu.loadHexFile(hex_file)) {
        return 1;
    }
    
    // Reset CPU
    cpu.resetCPU();
    
    // Print initial state
    std::cout << "\nInitial state:\n";
    cpu.printState();
    
    // Run for a few cycles (or until program ends)
    // std::cout << "\nExecuting instructions...\n";
    // cpu.runCycles(100);  // Execute 100 instructions
    for (int i = 0; i < 100; i++) {
        std::cout << "Cycle " << i << "\n";
        cpu.clockCycle();
        cpu.printState();
    }
    
    // Print final state
    std::cout << "\nFinal state:\n";
    cpu.printState();
    
    return 0;
}
