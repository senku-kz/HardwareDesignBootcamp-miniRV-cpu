#include <iostream>
#include <bitset>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Valu.h"

static void dump_state(Valu* alu) {
    std::cout << "    A=0x" << std::hex << (int)alu->operand_a << " B=0x" << (int)alu->operand_b
              << " function_3=" << std::bitset<3>(alu->function_3)
              << " function_7=" << std::bitset<7>(alu->function_7)
              << " => R=0x" << (int)alu->result
              << "\n";
}

int main(int argc, char** argv) {
    // Initialize Verilator
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);

    // Create DUT and VCD trace
    Valu* alu = new Valu;
    VerilatedVcdC* tfp = new VerilatedVcdC;
    alu->trace(tfp, 99);
    tfp->open("waveform_alu.vcd");

    uint64_t time = 0;
    auto eval_dump = [&](void) {
        alu->eval();
        tfp->dump(time++);
        dump_state(alu);
    };

    std::cout << "Testing ALU\n";
    std::cout << "==========\n\n";

    int passed = 0;

    // Test 1: ADD (simple)
    std::cout << "Test 1: ADD (5 + 7 = 12)\n";
    alu->operand_a = 5;
    alu->operand_b = 7;
    alu->function_3 = 0b000; // add
    alu->function_7 = 0b0000000; // add
    eval_dump();
    if (alu->result == 12) { std::cout << "  \xE2\x9C\x93 PASS\n\n"; passed++; } else { std::cerr << "  \xE2\x9C\x97 FAIL\n"; return 1; }

    // Test 2: SUB (7 - 7 = 0)
    std::cout << "Test 2: SUB (7 - 7 = 0)\n";
    alu->operand_a = 7;
    alu->operand_b = 7;
    alu->function_3 = 0b000; // sub
    alu->function_7 = 0b0100000; // sub
    eval_dump();
    if (alu->result == 0) { std::cout << "  \xE2\x9C\x93 PASS\n\n"; passed++; } else { std::cerr << "  \xE2\x9C\x97 FAIL\n"; return 1; }

    // Test 3: AND (0xAA & 0x0F = 0x0A)
    std::cout << "Test 3: AND (0xAA & 0x0F = 0x0A)\n";
    alu->operand_a = 0xAA;
    alu->operand_b = 0x0F;
    alu->function_3 = 0b111; // andi
    alu->function_7 = 0b0000000; // andi
    eval_dump();
    if (alu->result == 0x0A) { std::cout << "  \xE2\x9C\x93 PASS\n\n"; passed++; } else { std::cerr << "  \xE2\x9C\x97 FAIL\n"; return 1; }

    // Test 4: OR (0x00 | 0x00 = 0x00)
    std::cout << "Test 4: OR (0x00 | 0x00 = 0x00)\n";
    alu->operand_a = 0x00;
    alu->operand_b = 0x00;
    alu->function_3 = 0b110; // ori
    alu->function_7 = 0b0000000; // ori
    eval_dump();
    if (alu->result == 0x00) { std::cout << "  \xE2\x9C\x93 PASS\n\n"; passed++; } else { std::cerr << "  \xE2\x9C\x97 FAIL\n"; return 1; }

    // Test 5: ADD overflow (0xFF + 0x01 -> 0x00)
    std::cout << "Test 5: ADD overflow (0xFF + 0x01 -> 0x00)\n";
    alu->operand_a = 0xFFFFFFFF;
    alu->operand_b = 0x00000001;
    alu->function_3 = 0b000; // add
    alu->function_7 = 0b0000000; // add
    eval_dump();
    if (alu->result == 0x00000000) { std::cout << "  \xE2\x9C\x93 PASS\n\n"; passed++; } else { std::cerr << "  \xE2\x9C\x97 FAIL\n"; return 1; }

    // Test 6: SUB underflow (0x00 - 0x01 -> 0xFF)
    std::cout << "Test 6: SUB underflow (0x00 - 0x01 -> 0xFF)\n";
    alu->operand_a = 0x00000000;
    alu->operand_b = 0x00000001;
    alu->function_3 = 0b000; // sub
    alu->function_7 = 0b0100000; // sub
    eval_dump();
    if (alu->result == 0xFFFFFFFF) { std::cout << "  \xE2\x9C\x93 PASS\n\n"; passed++; } else { std::cerr << "  \xE2\x9C\x97 FAIL\n"; return 1; }

    // Test 7: Default stability (keep op AND then OR)
    std::cout << "Test 7: Operation switching stability\n";
    alu->operand_a = 0x55555555;
    alu->operand_b = 0x0F0F0F0F;
    alu->function_3 = 0b111; // andi
    alu->function_7 = 0b0000000; // andi
    eval_dump();
    if (alu->result != 0x00000000) { /* just sanity */ } else { std::cerr << "  \xE2\x9C\x97 FAIL\n"; return 1; }
    alu->function_3 = 0b110; // ori
    alu->function_7 = 0b0000000; // ori
    eval_dump();
    if (alu->result == (0x55555555 | 0x0F0F0F0F)) { std::cout << "  \xE2\x9C\x93 PASS\n\n"; passed++; } else { std::cerr << "  \xE2\x9C\x97 FAIL\n"; return 1; }

    // Cleanup
    tfp->close();
    delete tfp;
    delete alu;

    std::cout << "✅ All " << passed << " tests passed!\n";
    std::cout << "VCD file: waveform_alu.vcd\n";
    return 0;
}
