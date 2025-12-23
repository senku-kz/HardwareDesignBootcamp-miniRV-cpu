#include <iostream>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vprogram_counter.h"

int main(int argc, char** argv) {
    // Initialize Verilator
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);
    
    // Create module and VCD trace
    Vprogram_counter* pc = new Vprogram_counter;
    VerilatedVcdC* tfp = new VerilatedVcdC;
    pc->trace(tfp, 99);
    const char* filename = "waveform_pc.vcd";
    tfp->open(filename);
    
    uint64_t time = 0;
    uint32_t expected_pc_out;
    uint32_t real_pc_out;
    
    // Initialize signals
    pc->opcode = 0;
    pc->set_value = 0;
    pc->reset = 0;
    pc->clk = 0;
    

    // Test 1: Reset
    std::cout << "Test 1: Reset PC to 0\n";
    expected_pc_out = 0x00;
    pc->reset = 1;
    pc->clk = 0;
    pc->eval();
    tfp->dump(time++);
    
    pc->clk = 1;
    pc->eval();
    tfp->dump(time++);

    real_pc_out = pc->pc_out;
    
    if (real_pc_out != expected_pc_out) {
        std::cerr << "FAIL: Reset did not set pc_out to 0 (expected " << expected_pc_out << ", got " << real_pc_out << ")\n";
        return 1;
    }
    std::cout << "\t ok \t pc_out = " << expected_pc_out << "\n";
    

    // Test 2: Increment once
    std::cout << "\nTest 2: Increment once\n";
    expected_pc_out = 0x04;
    
    // Reset PC to 0
    pc->reset = 0;
    pc->clk = 0;
    pc->eval();
    tfp->dump(time++);
    
    pc->clk = 1;
    pc->eval();
    tfp->dump(time++);

    real_pc_out = pc->pc_out;
    
    if (real_pc_out != expected_pc_out) {
        std::cerr << "FAIL: First increment failed (expected " << expected_pc_out << ", got " << real_pc_out << ")\n";
        return 1;
    }   
    std::cout << "\t ok \t pc_out = " << expected_pc_out << "\n";
    

    // Test 3: Multiple increments
    std::cout << "\nTest 3: Multiple increments\n";
    expected_pc_out = 0x00;
    pc->reset = 1;
    pc->clk = 0;
    pc->eval();
    tfp->dump(time++);
    
    pc->clk = 1;
    pc->eval();
    tfp->dump(time++);

    pc->reset = 0;

    for (int i = 0; i < 5; i++) {
        pc->clk = 0;
        pc->eval();
        tfp->dump(time++);
        
        pc->clk = 1;
        pc->eval();
        tfp->dump(time++);

        real_pc_out = pc->pc_out;
        expected_pc_out = expected_pc_out + 4;
        if (real_pc_out != expected_pc_out) {
            std::cerr << "FAIL: Multiple increments failed (expected " << expected_pc_out << ", got " << real_pc_out << ")\n";
            return 1;
        }
        std::cout << "\t ok \t pc_out = " << expected_pc_out << "\n";
    }

    
    // Test 4: Branch instruction (set PC to specific value)
    std::cout << "\nTest 5: Branch to address 9\n";
    expected_pc_out = 0x0a;
    pc->opcode = 0x67;      // Branch opcode (jalr: 1100111 in binary = 0x67 in hex)
    pc->set_value = expected_pc_out;    // Target address
    pc->reset = 0;
    pc->clk = 0;
    pc->eval();
    tfp->dump(time++);
    
    pc->clk = 1;
    pc->eval();
    tfp->dump(time++);
    
    real_pc_out = pc->pc_out;
    if (real_pc_out != expected_pc_out) {
        std::cerr << "FAIL: Branch failed (expected " << expected_pc_out << ", got " << real_pc_out << ")\n";
        return 1;
    }
    std::cout << "\t ok \t pc_out = " << expected_pc_out << "\n";
    
    // Test 6: Return to normal increment after branch
    std::cout << "\nTest 5: Resume increment after branch\n";
    expected_pc_out = real_pc_out + 4;
    pc->opcode = 0x00;      // Normal mode
    pc->clk = 0;
    pc->eval();
    tfp->dump(time++);
    
    pc->clk = 1;
    pc->eval();
    tfp->dump(time++);
    
    real_pc_out = pc->pc_out;
    if (real_pc_out != expected_pc_out) {
        std::cerr << "FAIL: Increment after branch failed (expected " << expected_pc_out << ", got " << real_pc_out << ")\n";
        return 1;
    }
    std::cout << "\t ok \t pc_out = " << expected_pc_out << "\n";

    
    // Cleanup
    tfp->close();
    delete tfp;
    delete pc;
    
    std::cout << "\nok\tAll tests passed!\n";
    std::cout << "VCD file: " << filename << "\n";
    return 0;
}
