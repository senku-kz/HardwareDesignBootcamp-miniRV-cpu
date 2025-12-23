#include <iostream>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vprogram_counter.h"


uint32_t expected_pc_out = 0x00;
uint32_t real_pc_out = 0x00;

void initialize_signals(Vprogram_counter* pc) {
    pc->opcode = 0;
    pc->set_value = 0;
    pc->reset = 0;
    pc->clk = 0;
}

void clock_cycle(Vprogram_counter* pc, VerilatedVcdC* tfp, uint64_t& time) {
    pc->clk = 0;
    pc->eval();
    tfp->dump(time++);
    
    pc->clk = 1;
    pc->eval();
    tfp->dump(time++);
}


void reset_pc(Vprogram_counter* pc, VerilatedVcdC* tfp, uint64_t& time) {
    pc->reset = 1;
    clock_cycle(pc, tfp, time);
    pc->reset = 0;
}

bool test_reset(Vprogram_counter* pc, VerilatedVcdC* tfp, uint64_t& time) {
    initialize_signals(pc);
    expected_pc_out = 0x00;

    reset_pc(pc, tfp, time);

    real_pc_out = pc->pc_out;
    if (real_pc_out != expected_pc_out) {
        std::cerr << "FAIL: Reset did not set pc_out to 0 (expected " << expected_pc_out << ", got " << real_pc_out << ")\n";
        return false;
    }
    std::cout << "\t ok \t pc_out = " << expected_pc_out << "\n";
    return true;
}


bool test_increment_once(Vprogram_counter* pc, VerilatedVcdC* tfp, uint64_t& time) {
    initialize_signals(pc);
    reset_pc(pc, tfp, time);
    expected_pc_out = 0x04;

    clock_cycle(pc, tfp, time);

    real_pc_out = pc->pc_out;
    if (real_pc_out != expected_pc_out) {
        std::cerr << "FAIL: Increment did not increment pc_out (expected " << expected_pc_out << ", got " << real_pc_out << ")\n";
        return false;
    }
    std::cout << "\t ok \t pc_out = " << expected_pc_out << "\n";
    return true;
}


bool test_increment_multiple(Vprogram_counter* pc, VerilatedVcdC* tfp, uint64_t& time) {
    initialize_signals(pc);
    reset_pc(pc, tfp, time);
    expected_pc_out = 0x00;

    for (int i = 0; i < 5; i++) {
        clock_cycle(pc, tfp, time);
        real_pc_out = pc->pc_out;
        expected_pc_out = expected_pc_out + 4;
        if (real_pc_out != expected_pc_out) {
            std::cerr << "FAIL: Multiple increments failed (expected " << expected_pc_out << ", got " << real_pc_out << ")\n";
            return false;
        }
    }
    std::cout << "\t ok \t pc_out = " << expected_pc_out << "\n";
    return true;
}


bool test_jalr(Vprogram_counter* pc, VerilatedVcdC* tfp, uint64_t& time) {
    initialize_signals(pc);
    reset_pc(pc, tfp, time);
    expected_pc_out = 0x0a;

    pc->opcode = 0b1100111;  // JALR opcode (0x67)
    pc->set_value = expected_pc_out;

    clock_cycle(pc, tfp, time);

    real_pc_out = pc->pc_out;
    if (real_pc_out != expected_pc_out) {
        std::cerr << "FAIL: JALR did not set pc_out to 0x04 (expected " << expected_pc_out << ", got " << real_pc_out << ")\n";
        return false;
    }
    std::cout << "\t ok \t pc_out = " << expected_pc_out << "\n";
    return true;
}


bool test_increment_after_jalr(Vprogram_counter* pc, VerilatedVcdC* tfp, uint64_t& time) {
    initialize_signals(pc);
    reset_pc(pc, tfp, time);
    expected_pc_out = 0x0e;

    pc->opcode = 0b1100111;  // JALR opcode (0x67)
    pc->set_value = expected_pc_out;

    clock_cycle(pc, tfp, time); 

    expected_pc_out = expected_pc_out + 4;
    pc->opcode = 0x00;  // Normal mode
    clock_cycle(pc, tfp, time);

    real_pc_out = pc->pc_out;
    if (real_pc_out != expected_pc_out) {
        std::cerr << "FAIL: Increment after branch failed (expected " << expected_pc_out << ", got " << real_pc_out << ")\n";
        return false;
    }
    std::cout << "\t ok \t pc_out = " << expected_pc_out << "\n";
    return true;   
}


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
    test_reset(pc, tfp, time);
    

    // Test 2: Increment once
    std::cout << "\nTest 2: Increment once\n";
    test_increment_once(pc, tfp, time);
    

    // Test 3: Multiple increments
    std::cout << "\nTest 3: Multiple increments\n";
    test_increment_multiple(pc, tfp, time);

    
    // Test 4: Branch instruction (set PC to specific value)
    std::cout << "\nTest 5: Branch to address 9\n";
    test_jalr(pc, tfp, time);
    
    // Test 5: Return to normal increment after branch
    std::cout << "\nTest 5: Resume increment after branch\n";
    test_increment_after_jalr(pc, tfp, time);

    // Cleanup
    tfp->close();
    delete tfp;
    delete pc;
    
    std::cout << "\nok\tAll tests passed!\n";
    std::cout << "VCD file: " << filename << "\n";
    return 0;
}
