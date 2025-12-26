#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <bitset>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "VminiRV.h"

const std::string INSTRUCTION_MEMORY_FILE = "logisim-bin/test-pc4.hex";

// Helper function to print instruction name
const char* instruction_name(uint32_t instr) {
    uint8_t opcode = instr & 0x7F;
    uint8_t funct3 = (instr >> 12) & 0x7;
    
    switch (opcode) {
        case 0x33: return "ADD";
        case 0x13: return "ADDI";
        case 0x37: return "LUI";
        case 0x03:
            if (funct3 == 0x2) return "LW";
            if (funct3 == 0x4) return "LBU";
            return "LOAD?";
        case 0x23:
            if (funct3 == 0x2) return "SW";
            if (funct3 == 0x0) return "SB";
            return "STORE?";
        case 0x67: return "JALR";
        default: return "UNKNOWN";
    }
}

// Helper function to print register value (we'll need to access internal state)
void print_registers(VminiRV* cpu, const char* label) {
    std::cout << "  " << label << ":\n";
    // Note: In a real test, we would need to expose register values
    // For now, we'll check through memory/PC observations
}

// Test helper: Run CPU for N cycles
void run_cycles(VminiRV* cpu, VerilatedVcdC* tfp, uint64_t& time, int cycles) {
    for (int i = 0; i < cycles; i++) {
        cpu->clk = 0;
        cpu->eval();
        tfp->dump(time++);
        
        cpu->clk = 1;
        cpu->eval();
        tfp->dump(time++);
    }
}

bool compare_cpus(Vmain* designed_cpu, sCPU* golden_cpu, int cycle) {
    bool match = true;
    
    // Compare PC (4-bit value stored in uint8_t)
    uint8_t designed_pc = designed_cpu->pc_debug;
    uint8_t golden_pc = golden_cpu->getPc();
    
    if (designed_pc != golden_pc) {
        std::cout << "  err Cycle " << std::setw(3) << cycle << ": PC mismatch - Designed CPU: " 
                  << std::setw(3) << (int)designed_pc << ", Golden CPU: " << std::setw(3) << (int)golden_pc << "\n";
        match = false;
    }
    
    // Compare all registers
    for (int i = 0; i < 4; i++) {
        uint8_t designed_reg = 0;
        uint8_t golden_reg = golden_cpu->getRegister(i);
        
        // Get register value from hardware CPU
        switch(i) {
            case 0: designed_reg = designed_cpu->reg0_debug; break;
            case 1: designed_reg = designed_cpu->reg1_debug; break;
            case 2: designed_reg = designed_cpu->reg2_debug; break;
            case 3: designed_reg = designed_cpu->reg3_debug; break;
        }
                
        if (designed_reg != golden_reg) {
            std::cout << "  err Cycle " << std::setw(3) << cycle << ": R" << i << " mismatch - Designed CPU: " 
                      << std::setw(3) << (int)designed_reg << ", Golden CPU: " << std::setw(3) << (int)golden_reg << "\n";
            match = false;
        }
    }
    
    return match;
}

bool compare_cpus(Vmain* designed_cpu, sCPU* golden_cpu, int cycle) {
    bool match = true;
    
    // Compare PC (4-bit value stored in uint8_t)
    uint8_t designed_pc = designed_cpu->pc_;
    uint8_t golden_pc = golden_cpu->getPc();
    
    if (designed_pc != golden_pc) {
        std::cout << "  err Cycle " << std::setw(3) << cycle << ": PC mismatch - Designed CPU: " 
                  << std::setw(3) << (int)designed_pc << ", Golden CPU: " << std::setw(3) << (int)golden_pc << "\n";
        match = false;
        throw std::runtime_error("PC mismatch");
    }
    
    // Compare all registers

    uint32_t designed_registers[15 = designed_cpu->registers;
    uint32_t golden_registers[15 = golden_cpu->getRegisters();

    for (int i = 0; i < 15; i++) {
                
        if (designed_reg != golden_reg) {
            std::cout << "  err Cycle " << std::setw(3) << cycle << ": R" << i << " mismatch - Designed CPU: " 
                      << std::setw(3) << (int)designed_reg << ", Golden CPU: " << std::setw(3) << (int)golden_reg << "\n";
            match = false;
        }
    }
    
    return match;
}

int main(int argc, char** argv) {
    // Initialize Verilator
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);
    
    // Create CPU and VCD trace
    VminiRV* cpu = new VminiRV;
    VerilatedVcdC* tfp = new VerilatedVcdC;
    cpu->trace(tfp, 99);
    tfp->open("waveform_miniRV.vcd");
    
    uint64_t time = 0;
    bool test_result = false;
    int test_count = 0;
    int test_success = 0;
    
    std::cout << "Testing miniRV CPU\n";
    std::cout << "==================\n\n";
    
    // Initialize CPU
    cpu->clk = 0;
    cpu->reset = 0;
    cpu->eval();
    tfp->dump(time++);
  
    


    std::cout << "\n";
    
    // Cleanup
    tfp->close();
    delete tfp;
    delete cpu;
    
    if (test_success == test_count) {
        std::cout << "✅ All " << test_success << " tests passed!\n";
    } else {
        std::cout << "⚠️  " << test_count - test_success << " tests need implementation\n";
    }
    
    return (test_success == test_count) ? 0 : 1;
}

