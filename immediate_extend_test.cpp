#include <iostream>
#include <bitset>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vimmediate_extend.h"


int main(int argc, char** argv) {
    // Initialize Verilator
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);

    // Create DUT and VCD trace
    Vimmediate_extend* dut = new Vimmediate_extend;
    VerilatedVcdC* tfp = new VerilatedVcdC;
    dut->trace(tfp, 99);
    tfp->open("waveform_imm.vcd");

    std::cout << "Testing immediate_extend (zero-extend 12->32)\n";
    std::cout << "==========================================\n\n";

    uint64_t time = 0;

    // Test values with step 0x8 (byte alignment: 0x0, 0x8, 0x10, 0x18, ..., 0xFF8) to verify zero-extend into 32 bits
    for (uint16_t imm_in = 1; imm_in <= 0xFF8; imm_in *= 0x8) {
        dut->imm_in = imm_in;
        dut->eval();
        tfp->dump(time++);

        uint32_t expected = static_cast<uint32_t>(imm_in);
        uint32_t actual = dut->imm_out;

        if (actual != expected) {
            std::cerr << "  FAIL \t imm_in=0b" << std::bitset<12>(imm_in)
                      << " -> imm_out=0x" << std::hex << actual
                      << std::dec << " (expected 0x" << std::hex << expected << std::dec << ")\n";
            return 1;
        }

        std::cout << "  OK   \t imm_in=0b" << std::bitset<12>(imm_in)
                  << " -> imm_out=0x" << std::hex << actual
                  << std::dec << " (expected 0x" << std::hex << expected << std::dec << ")"
                  << "\n";
    }


    // Cleanup
    tfp->close();
    delete tfp;
    delete dut;

    std::cout << "\n✅ All zero-extend test cases passed!\n";
    std::cout << "VCD file: waveform_imm.vcd\n";
    return 0;
}
