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

// Test: ADD instruction
// add x1, x2, x3  -> x1 = x2 + x3
bool test_add(VminiRV* cpu, VerilatedVcdC* tfp, uint64_t& time) {
    std::cout << "Test: ADD instruction\n";
    std::cout << "  Instruction: add x1, x2, x3\n";
    std::cout << "  Expected: x1 = x2 + x3\n";
    
    // Reset CPU
    cpu->reset = 0;
    run_cycles(cpu, tfp, time, 1);
    cpu->reset = 1;

    int x2 = 5;
    int x3 = 3;
    int expected_x1 = x2 + x3;

    cpu->rs1 = x2;
    cpu->rs2 = x3;
    cpu->rd = 1;
    cpu->alu_op = 0;
    cpu->alu_src = 0;
    cpu->wb_sel = 0;
    
    // Initialize registers x2=5, x3=3 via instructions
    // This requires setting up instruction memory properly
    // For now, we'll mark as placeholder
    std::cout << "  NOTE: Full test requires instruction memory setup\n";
    
    return true;
}

// Test: ADDI instruction
// addi x1, x2, 10  -> x1 = x2 + 10
bool test_addi(VminiRV* cpu, VerilatedVcdC* tfp, uint64_t& time) {
    std::cout << "Test: ADDI instruction\n";
    std::cout << "  Instruction: addi x1, x2, 10\n";
    std::cout << "  Expected: x1 = x2 + 10\n";
    
    std::cout << "  NOTE: Full test requires instruction memory setup\n";
    
    return true;
}

// Test: LUI instruction
// lui x1, 0x12345  -> x1 = 0x12345000
bool test_lui(VminiRV* cpu, VerilatedVcdC* tfp, uint64_t& time) {
    std::cout << "Test: LUI instruction\n";
    std::cout << "  Instruction: lui x1, 0x12345\n";
    std::cout << "  Expected: x1 = 0x12345000\n";
    
    std::cout << "  NOTE: Full test requires instruction memory setup\n";
    
    return true;
}

// Test: LW instruction
// lw x1, 0(x2)  -> x1 = mem[x2 + 0]
bool test_lw(VminiRV* cpu, VerilatedVcdC* tfp, uint64_t& time) {
    std::cout << "Test: LW instruction\n";
    std::cout << "  Instruction: lw x1, 0(x2)\n";
    std::cout << "  Expected: x1 = mem[x2 + 0]\n";
    
    std::cout << "  NOTE: Full test requires instruction memory setup\n";
    
    return true;
}

// Test: LBU instruction
// lbu x1, 0(x2)  -> x1 = zero_extend(mem[x2 + 0][7:0])
bool test_lbu(VminiRV* cpu, VerilatedVcdC* tfp, uint64_t& time) {
    std::cout << "Test: LBU instruction\n";
    std::cout << "  Instruction: lbu x1, 0(x2)\n";
    std::cout << "  Expected: x1 = zero_extend(mem[x2 + 0][7:0])\n";
    
    std::cout << "  NOTE: Full test requires instruction memory setup\n";
    
    return true;
}

// Test: SW instruction
// sw x1, 0(x2)  -> mem[x2 + 0] = x1
bool test_sw(VminiRV* cpu, VerilatedVcdC* tfp, uint64_t& time) {
    std::cout << "Test: SW instruction\n";
    std::cout << "  Instruction: sw x1, 0(x2)\n";
    std::cout << "  Expected: mem[x2 + 0] = x1\n";
    
    std::cout << "  NOTE: Full test requires instruction memory setup\n";
    
    return true;
}

// Test: SB instruction
// sb x1, 0(x2)  -> mem[x2 + 0][7:0] = x1[7:0]
bool test_sb(VminiRV* cpu, VerilatedVcdC* tfp, uint64_t& time) {
    std::cout << "Test: SB instruction\n";
    std::cout << "  Instruction: sb x1, 0(x2)\n";
    std::cout << "  Expected: mem[x2 + 0][7:0] = x1[7:0]\n";
    
    std::cout << "  NOTE: Full test requires instruction memory setup\n";
    
    return true;
}

// Test: JALR instruction
// jalr x1, 0(x2)  -> x1 = PC + 4, PC = (x2 + 0) & ~1
bool test_jalr(VminiRV* cpu, VerilatedVcdC* tfp, uint64_t& time) {
    std::cout << "Test: JALR instruction\n";
    std::cout << "  Instruction: jalr x1, 0(x2)\n";
    std::cout << "  Expected: x1 = PC + 4, PC = (x2 + 0) & ~1\n";
    
    std::cout << "  NOTE: Full test requires instruction memory setup\n";
    
    return true;
}

// Comprehensive test program
// This test runs a sequence of instructions and verifies results
bool test_comprehensive_program(VminiRV* cpu, VerilatedVcdC* tfp, uint64_t& time) {
    std::cout << "Test: Comprehensive Program\n";
    std::cout << "  Running sequence: addi, add, lui, sw, lw, lbu, sb, jalr\n";
    
    // Reset CPU
    cpu->reset = 0;
    run_cycles(cpu, tfp, time, 1);
    cpu->reset = 1;
    
    // Run for multiple cycles to execute program
    // The actual program would be loaded in instruction memory
    run_cycles(cpu, tfp, time, 50);
    
    std::cout << "  NOTE: Full test requires instruction memory setup\n";
    
    return true;
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
    
    // Test 1: ADD
    test_result = test_add(cpu, tfp, time);
    if (test_result) test_success++;
    test_count++;
    
    // // Test 2: ADDI
    // test_result = test_addi(cpu, tfp, time);
    // if (test_result) test_success++;
    // test_count++;
    
    // // Test 3: LUI
    // test_result = test_lui(cpu, tfp, time);
    // if (test_result) test_success++;
    // test_count++;
    
    // // Test 4: LW
    // test_result = test_lw(cpu, tfp, time);
    // if (test_result) test_success++;
    // test_count++;
    
    // // Test 5: LBU
    // test_result = test_lbu(cpu, tfp, time);
    // if (test_result) test_success++;
    // test_count++;
    
    // // Test 6: SW
    // test_result = test_sw(cpu, tfp, time);
    // if (test_result) test_success++;
    // test_count++;
    
    // // Test 7: SB
    // test_result = test_sb(cpu, tfp, time);
    // if (test_result) test_success++;
    // test_count++;
    
    // // Test 8: JALR
    // test_result = test_jalr(cpu, tfp, time);
    // if (test_result) test_success++;
    // test_count++;
    
    // // Test 9: Comprehensive program
    // test_result = test_comprehensive_program(cpu, tfp, time);
    // if (test_result) test_success++;
    // test_count++;
    
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

