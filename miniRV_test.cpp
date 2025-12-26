#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <bitset>
#include <cstring>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "VminiRV.h"
#include "golden_model_cpu.h"

size_t REGISTER_LIMIT = 16;
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


bool compare_cpus(VminiRV* miniRV_cpu, GoldenModelCPU* golden_cpu, int cycle) {
    
    // Compare PC (32-bit value)
    uint32_t designed_pc = miniRV_cpu->pc_;
    uint32_t golden_pc = golden_cpu->pc;
    
    if (designed_pc != golden_pc) {
        std::cout << "  err Cycle " << std::setw(3) << cycle << ": PC mismatch - Designed CPU: 0x" 
                  << std::hex << std::setfill('0') << std::setw(8) << designed_pc << std::dec
                  << ", Golden CPU: 0x" << std::hex << std::setfill('0') << std::setw(8) << golden_pc << std::dec << "\n";
        throw std::runtime_error("PC mismatch");
    }
    
    // Compare all registers
    uint32_t miniRV_registers[REGISTER_LIMIT];
    uint32_t golden_registers[REGISTER_LIMIT];
    memcpy(miniRV_registers, miniRV_cpu->registers, sizeof(uint32_t) * REGISTER_LIMIT);
    memcpy(golden_registers, golden_cpu->registers, sizeof(uint32_t) * REGISTER_LIMIT);

    for (int i = 0; i < REGISTER_LIMIT; i++) {
        if (miniRV_registers[i] != golden_registers[i]) {
            std::cout << "  err Cycle " << std::setw(3) << cycle << ": R" << i << " mismatch - Designed CPU: 0x" 
                      << std::hex << std::setfill('0') << std::setw(8) << miniRV_registers[i] << std::dec
                      << ", Golden CPU: 0x" << std::hex << std::setfill('0') << std::setw(8) << golden_registers[i] << std::dec << "\n";
            throw std::runtime_error("Register mismatch");
        }
    }

    return true;
}

int main(int argc, char** argv) {
    uint64_t time = 0;
    bool test_result = false;
    int test_count = 0;
    int test_success = 0;
 
    // Initialize Verilator
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);
    
    // Create CPU and VCD trace
    VminiRV* miniRV_cpu = new VminiRV;
    VerilatedVcdC* tfp = new VerilatedVcdC;
    miniRV_cpu->trace(tfp, 99);
    tfp->open("waveform_miniRV.vcd");
    
    // Create golden model CPU
    GoldenModelCPU golden_cpu;
    golden_cpu.loadHexFile(INSTRUCTION_MEMORY_FILE);
    golden_cpu.resetCPU();
    golden_cpu.clockCycle();
  
  
    // Initialize CPU
    miniRV_cpu->clk = 0;
    miniRV_cpu->reset = 0;
    miniRV_cpu->eval();
    tfp->dump(time++);
  
    
    std::cout << "Testing miniRV CPU\n";
    std::cout << "==================\n\n";
  
    for (int i = 0; i < 6000; i++) {
        golden_cpu.clockCycle();
        run_cycles(miniRV_cpu, tfp, time, 1);

        if (!compare_cpus(miniRV_cpu, &golden_cpu, i)) {
            std::cout << "  err Cycle " << std::setw(3) << i << ": CPU mismatch\n";
            break;
        }
    }

    std::cout << "\n";
    
    // Cleanup
    tfp->close();
    delete tfp;
    delete miniRV_cpu;
    
    if (test_success == test_count) {
        std::cout << "✅ All " << test_success << " tests passed!\n";
    } else {
        std::cout << "⚠️  " << test_count - test_success << " tests need implementation\n";
    }
    
    return (test_success == test_count) ? 0 : 1;
}

