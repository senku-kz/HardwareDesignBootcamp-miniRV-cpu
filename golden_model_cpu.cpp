#include "golden_model_cpu.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>
#include <bitset>
#include <stdexcept>

// Golden Model CPU for miniRV (RISC-V RV32E subset)
// This is a functional C++ model that executes instructions from hex files
int CYCLE_LIMIT = 6000;

bool DEBUG_MODE = false;

// GoldenModelCPU class implementation

// Constructor
GoldenModelCPU::GoldenModelCPU() : clock(false), reset(false), pc(0) {
    // Initialize registers (x0 is always 0, others can be 0 initially)
    for (int i = 0; i < REGISTER_LIMIT; i++) {
        registers[i] = 0;
    }
    
    // Initialize memories
    memset(imem, 0, sizeof(imem));
    memset(dmem, 0, sizeof(dmem));
}
    

// Load instructions from hex file (Logisim format)
bool GoldenModelCPU::loadHexFile(const std::string& filename) {
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
        // base_addr = base_addr >> 2;  // Convert byte address to word index
        
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
                dmem[word_addr] = instr;
            } else {
                std::cerr << "Warning: Instruction address " << word_addr 
                            << " exceeds memory size" << std::endl;
                throw std::runtime_error("Memory address out of bounds");
            }
            
            offset++;
        }
    }
    
    file.close();
    return true;
}


// Reset CPU
void GoldenModelCPU::resetCPU() {
    reset = true;
    pc = 0;
    for (int i = 0; i < REGISTER_LIMIT; i++) {
        registers[i] = 0;
    }
    reset = false;
}


// Execute one instruction (single cycle)
bool GoldenModelCPU::executeInstruction() {
    // Fetch instruction
    uint32_t word_index = pc >> 2;
    if (word_index >= IMEM_SIZE) {
        std::cerr << "Error: PC out of bounds: 0x" << std::hex << pc << std::dec << std::endl;
        throw std::runtime_error("PC out of bounds");
        return false;
    }
    
    uint32_t instr = imem[word_index];

    if (DEBUG_MODE) {
        std::cout << "PC: 0x" << std::hex << pc << std::dec << std::endl;
        std::cout << "Instruction: 0x" << std::hex << std::setfill('0') << std::setw(8) << instr << std::dec << std::endl;
        std::cout << "Instruction: 0b" << std::bitset<32>(instr) << std::endl;
    }
    
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
    
    // Execute instruction based on opcode
    switch (opcode) {
        case 0x33: {  // R-Type: ADD
            if (DEBUG_MODE) {
                std::cout << "ADD: rd = x" << (int)rd 
                << ", rs1 = x" << (int)rs1 
                << ", rs2 = x" << (int)rs2 
                << std::endl;
            }

            if (funct3 == 0x0 && funct7 == 0x0) {
                if (rd < REGISTER_LIMIT && rs1 < REGISTER_LIMIT && rs2 < REGISTER_LIMIT) {
                    registers[rd] = registers[rs1] + registers[rs2];
                    if (DEBUG_MODE) {
                        std::cout << "ADD: rd = x" << (int)rd << " = 0x" << std::hex << registers[rd] << std::dec << std::endl;
                    }
                } else {
                    std::cerr << "Error: Illegal register: rd = " << (int)rd << ", rs1 = " << (int)rs1 << ", rs2 = " << (int)rs2 << std::endl;
                    // throw std::runtime_error("Illegal register");
                }
            } else {
                std::cerr << "Error: Illegal function: funct3 = 0b" << std::bitset<3>(funct3) << ", funct7 = 0b" << std::bitset<7>(funct7) << std::endl;
                throw std::runtime_error("Illegal function");
            }
            break;
        }
        
        case 0x13: {  // I-Type: ADDI
            if (DEBUG_MODE) {
                std::cout << "ADDI: rd = x" << (int)rd 
                << ", rs1 = x" << (int)rs1 << "(0x" << std::hex << registers[rs1] << std::dec << ")"
                << ", imm_i = " << imm_i << " (0x" << std::hex << imm_i << std::dec << ")" << std::endl;
            }
            if (funct3 == 0x0) {
                if (rd < REGISTER_LIMIT && rs1 < REGISTER_LIMIT) {
                    registers[rd] = registers[rs1] + imm_i;
                    if (DEBUG_MODE) {
                        std::cout << "ADDI: rd = x" << (int)rd << " = 0x" << std::hex << registers[rd] << std::dec << std::endl;
                    }
                } else {
                    std::cerr << "Error: Illegal register: rd = " << (int)rd << ", rs1 = " << (int)rs1 << std::endl;
                    throw std::runtime_error("Illegal register");
                }
            } else {
                std::cerr << "Error: Illegal function: funct3 = 0b" << std::bitset<3>(funct3) << ", funct7 = 0b" << std::bitset<7>(funct7) << std::endl;
                throw std::runtime_error("Illegal function");
            }
            break;
        }
        
        case 0x37: {  // U-Type: LUI
            if (DEBUG_MODE) {
                std::cout << "LUI: rd = x" << (int)rd 
                << " <- imm_u = 0x" << std::hex << imm_u << std::dec << std::endl;
            }
            if (rd < REGISTER_LIMIT && rd != 0) {
                registers[rd] = imm_u;
            } else {
                std::cerr << "Error: Illegal register: rd = " << (int)rd << std::endl;
                throw std::runtime_error("Illegal register");
            }
            break;
        }
        
        case 0x03: {  // I-Type: Load instructions
            if (DEBUG_MODE) {
                std::cout << "LW: rd = x" << (int)rd 
                << ", rs1 = x" << (int)rs1 
                << ", imm_i = " << imm_i << " (0x" << std::hex << imm_i << std::dec << ")" 
                << ", funct3 = 0b" << std::bitset<3>(funct3) << std::dec
                << ", funct7 = 0b" << std::bitset<7>(funct7) << std::dec
                << std::endl;
            }
            if (rd < REGISTER_LIMIT && rs1 < REGISTER_LIMIT) {
                uint32_t addr = registers[rs1] + imm_i;
                uint32_t word_addr = addr >> 2;
                
                if (word_addr < DMEM_SIZE) {
                    // Read 32-bit word directly (word-addressable memory)
                    uint32_t dmem_rdata = dmem[word_addr];
                    
                    if (funct3 == 0x2) {  // LW - Load word (32-bit)
                        if (DEBUG_MODE) {
                            std::cout << "LW: rd = x" << (int)rd 
                                        << ", rs1 = x" << (int)rs1 
                                        << ", imm_i = " << imm_i 
                                        << ", addr = 0x" << std::hex << addr << std::dec 
                                        << " (word_index: 0x" << std::hex << word_addr << std::dec << ")"
                                        << std::endl;
                        }
                        if (rd != 0) {
                            registers[rd] = dmem_rdata;
                        }
                    } else if (funct3 == 0b100) {  // LBU - Load byte unsigned (8-bit)
                        //Load Byte Unsigned: Loads 8 bits from memory and zero-extends them to 32 bits.
                        //addr = R[rs1] + imm; R[rd] = {24'b0, M[addr][7:0]}
                        if (DEBUG_MODE) {
                            std::cout << "LBU: rd = x" << (int)rd 
                                        << ", rs1 = x" << (int)rs1 
                                        << ", imm_i = " << imm_i 
                                        << ", addr = 0x" << std::hex << addr << std::dec 
                                        << " (word_index: 0x" << std::hex << word_addr << std::dec << ")"
                                        << std::endl;
                        }
                        if (rd != 0) {
                            registers[rd] = (uint32_t)(dmem_rdata & 0x000000FF);
                        }
                    } else {
                        std::cerr << "Error: Illegal function: funct3 = 0b" << std::bitset<3>(funct3) << std::endl;
                        throw std::runtime_error("Illegal function");
                    }
                }
            }
            break;
        }
        
        case 0x23: {  // S-Type: Store instructions
            if (DEBUG_MODE) {
                std::cout << "STORE: rs1 = x" << (int)rs1 << "(0x" << std::hex << registers[rs1] << std::dec << ")"
                << ", rs2 = x" << (int)rs2 << "(0x" << std::hex << registers[rs2] << std::dec << ")"
                << ", imm_s = 0x" << std::hex << imm_s << std::dec 
                << ", funct3 = 0b" << std::bitset<3>(funct3) << std::dec
                << ", funct7 = 0b" << std::bitset<7>(funct7) << std::dec
                << std::endl;
            }
            
            if (rs1 < REGISTER_LIMIT && rs2 < REGISTER_LIMIT) {
                uint32_t addr = registers[rs1] + imm_s;
                uint32_t word_addr = addr >> 2;
                uint32_t dmem_wdata = registers[rs2];

                if (word_addr < DMEM_SIZE) {
                    if (funct3 == 0x2) { // SW - Store word (32-bit)
                        // Store full 32-bit word
                        // funct3 = 0b010 (SW)
                        dmem[word_addr] = dmem_wdata;
                        if (DEBUG_MODE) {
                            std::cout << "STORE: dmem[0x" << std::hex << addr << std::dec << "] = 0x" << std::hex << dmem_wdata << std::dec 
                            << " (real addr >> 2: 0x" << std::hex << word_addr << std::dec << ")"
                            << std::endl;
                        }
                    } else if (funct3 == 0b000) {  // SB - Store byte (8-bit)
                        // Store Byte: Stores the lowest 8 bits of a register into memory.
                        // addr = R[rs1] + imm; M[addr] = R[rs2][7:0]
                        if (DEBUG_MODE) {
                            std::cout << "SB: dmem[0x" << std::hex << addr << std::dec << "] = 0x" << std::hex << dmem_wdata << std::dec 
                            << " (real addr >> 2: 0x" << std::hex << word_addr << std::dec << ")"
                            << std::endl;
                        }
                        dmem[word_addr] = (uint32_t)(dmem_wdata & 0x000000FF);
                    } else {
                        std::cerr << "Error: Illegal function: funct3 = 0b" << std::bitset<3>(funct3) << std::endl;
                        throw std::runtime_error("Illegal function");
                    }
                } else {
                    std::cerr << "Error: Illegal address: word_addr = 0x" << std::hex << word_addr << std::dec << std::endl;
                    throw std::runtime_error("Illegal address");
                }
            } else {
                std::cerr << "Error: Illegal register: rs1 = " << (int)rs1 << ", rs2 = " << (int)rs2 << std::endl;
                throw std::runtime_error("Illegal register");
            }
            break;
        }
        
        case 0x67: {  // I-Type: JALR
            if (DEBUG_MODE) {
                std::cout << "JALR: rd = x" << (int)rd 
                << ", rs1 = x" << (int)rs1 
                << ", imm_i = " << imm_i << std::endl;
            }
            
            if (funct3 == 0x0) {
                if (rd < REGISTER_LIMIT && rs1 < REGISTER_LIMIT) {
                    next_pc = (registers[rs1] + imm_i) & 0xFFFFFFFE;  // Clear LSB
                    registers[rd] = pc_plus4;
                    // next_pc = target;
                } else {
                    std::cerr << "Error: Illegal register: rd = " << (int)rd << ", rs1 = " << (int)rs1 << std::endl;
                    // throw std::runtime_error("Illegal register");
                }
            }
            if (DEBUG_MODE) {
                std::cout << "JALR: pc_new = 0x" << std::hex << next_pc << std::dec 
                << ", pc_saved pc + 4 = 0x" << std::hex << pc_plus4 << std::dec 
                << " in register x" << (int)rd << std::endl;
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
    registers[0] = 0;
    
    return true;
}


// Clock cycle: execute one instruction
void GoldenModelCPU::clockCycle() {
    if (!reset) {
        clock = true;
        executeInstruction();
        clock = false;
    }
}


// Run CPU for N cycles
void GoldenModelCPU::runCycles(int cycles) {
    for (int i = 0; i < cycles; i++) {
        clockCycle();
    }
}


// Print CPU state
void GoldenModelCPU::printState() {
    std::cout << "CPU State:\n";
    std::cout << "  PC: 0x" << std::hex << std::setfill('0') << std::setw(8) << pc << std::dec << "\n";
    std::cout << "  Registers:\n";
    for (int i = 0; i < REGISTER_LIMIT; i++) {
        std::cout << "\t x" << i << ": 0x" << std::hex << std::setfill('0') << std::setw(8) 
                    << registers[i] << std::dec;
        if (i % 4 == 3) std::cout << "\n";
        else std::cout << "  ";
    }
    if (REGISTER_LIMIT % 4 != 0) std::cout << "\n";
}


// Get instruction at address
uint32_t GoldenModelCPU::getInstruction(uint32_t addr) {
    uint32_t word_index = addr >> 2;
    if (word_index < IMEM_SIZE) {
        return imem[word_index];
    }
    return 0;
}


// Read data from memory
uint32_t GoldenModelCPU::readMem(uint32_t addr) {
    uint32_t word_addr = addr >> 2;
    if (word_addr < DMEM_SIZE) {
        return dmem[word_addr];  // Direct word access (32-bit data width)
    }
    return 0;
}
