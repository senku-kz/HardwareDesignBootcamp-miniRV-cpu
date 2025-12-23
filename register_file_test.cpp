#include <iostream>
#include <iomanip>
#include <bitset>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vregister_file.h"

void print_registers(Vregister_file* rf, const std::string& msg) {
    std::cout << "  " << msg << "\n";
    std::cout << "    rdata1[" << (int)rf->raddr1 << "] = 0b" << std::bitset<32>((int)rf->rdata1) << "\n";
    std::cout << "    rdata2[" << (int)rf->raddr2 << "] = 0b" << std::bitset<32>((int)rf->rdata2) << "\n";
}

void clock_cycle(Vregister_file* rf, VerilatedVcdC* tfp, uint64_t& time) {
    rf->clk = 0;
    rf->eval();
    tfp->dump(time++);
    
    rf->clk = 1;
    rf->eval();
    tfp->dump(time++);
}

int main(int argc, char** argv) {
    // Initialize Verilator
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);
    
    // Create module and VCD trace
    Vregister_file* rf = new Vregister_file;
    VerilatedVcdC* tfp = new VerilatedVcdC;
    rf->trace(tfp, 99);
    tfp->open("waveform_register_file.vcd");
    
    uint64_t time = 0;
    
    // Test 1: Register 0 should be 0
    std::cout << "Test 1: Register 0 should be 0\n";
    uint8_t expected_result = 0;
    uint8_t actual_result = 0;

    rf->clk = 0;
    rf->reset = 0;
    rf->we = 0;
    rf->waddr = 0;
    rf->wdata = expected_result;
    rf->raddr1 = 0;
    rf->raddr2 = 0;
    clock_cycle(rf, tfp, time);

    actual_result = rf->rdata1;
    if (actual_result != expected_result) {
        std::cerr << "  FAIL \t R0 should be 0, got " 
        << ": 0x" << std::setfill('0') << std::setw(8) << std::hex << actual_result 
        << " (expected 0x" << std::setfill('0') << std::setw(8) << std::hex << expected_result << ")\n";
        return 1;
    }
    std::cout << "  OK   \t Register R0 was set to 0, read back as " 
    << ": 0x" << std::setfill('0') << std::setw(8) << std::hex << actual_result 
    << " (expected 0x" << std::setfill('0') << std::setw(8) << std::hex << expected_result << ")\n\n";
    

    // Test 2: Write to multiple registers
    std::cout << "Test 2: Write to registers r1, r2, r3, r4\n";
    
    // Write to r1, r2, r3, r4
    uint32_t test_values[4] = {
        0x10001010, 
        0x10010000, 
        0x10100000,
        0x10110001
    };

    // Write to registers r1, r2, r3, r4
    for (int i = 1; i <= sizeof(test_values) / sizeof(test_values[0]); i++) {
        rf->we = 1;
        rf->waddr = i;
        rf->wdata = test_values[i - 1];
        clock_cycle(rf, tfp, time);
        std::cout << "  Written R" << i << " = 0x" << std::hex << test_values[i - 1] << std::dec << "\n";
    }
    
    // Read back all registers
    std::cout << "\n  Reading back:\n";
    rf->we = 0;
    for (int i = 0; i < sizeof(test_values) / sizeof(test_values[0]); i++) {
        rf->raddr1 = i + 1;
        clock_cycle(rf, tfp, time);
        
        uint32_t expected_result = test_values[i];
        uint32_t actual_result = rf->rdata1;

        if (actual_result != expected_result) {
            std::cerr << "  FAIL \t R" << i + 1
            << " should be 0x" << std::hex << expected_result 
            << ", got 0x" << std::hex << actual_result 
            << "\n";
            return 1;
        }
        std::cout << "  OK   \t R" << i + 1
        << " = 0x" << std::hex << actual_result 
        << " (expected 0x" << std::hex << expected_result << ")\n";
    }
    std::cout << "\n";
    

    // Test 3: Read two registers simultaneously
    std::cout << "Test 3: Simultaneous dual-port read (rs1 and rs2)\n";
    // Initialize expected values
    int r1_index = 1;
    int r2_index = 2;

    // Read from r1 and r2
    rf->raddr1 = r1_index;
    rf->raddr2 = r2_index;
    clock_cycle(rf, tfp, time);
    
    uint32_t expected_result_rdata1 = test_values[r1_index - 1];
    uint32_t expected_result_rdata2 = test_values[r2_index - 1];
    uint32_t actual_result_rdata1 = rf->rdata1;
    uint32_t actual_result_rdata2 = rf->rdata2;
    
    if (actual_result_rdata1 != expected_result_rdata1 || actual_result_rdata2 != expected_result_rdata2) {
        std::cerr << "  FAIL \t R1 and R2 should be 0x" << std::hex << expected_result_rdata1 << " and 0x" << std::hex << expected_result_rdata2 
        << ", got 0x" << std::hex << actual_result_rdata1 << " and 0x" << std::hex << actual_result_rdata2 
        << "\n";
        return 1;
    }
    std::cout << "  OK   \t R1 and R2 = 0x" << std::hex << actual_result_rdata1 << " and 0x" << std::hex << actual_result_rdata2 
    << " (expected 0x" << std::hex << expected_result_rdata1 << " and 0x" << std::hex << expected_result_rdata2 << ")\n\n";
    

    // // Test 5: Write enable off - no write
    // std::cout << "Test 5: Write enable disabled (we=0)\n";
    // rf->we = 0;
    // rf->rd = 1;
    // rf->wd = 0xFF;  // Try to write with we=0
    // clock_cycle(rf, tfp, time);
    
    // rf->rs1 = 1;
    // rf->eval();
    // tfp->dump(time++);

    // expected_result = test_values[rf->rd];
    // actual_result = rf->rs1_out;
    
    // if (expected_result != actual_result) {  // Should still be old value
    //     std::cerr << "  ✗ FAIL: Register changed when we=0\n";
    //     return 1;
    // }
    // std::cout << "  ✓ r1 unchanged (still 0x" << std::hex << (int)actual_result << std::dec << ")\n\n";
    
    // // Test 6: Overwrite register
    // std::cout << "Test 6: Overwrite existing register\n";
    // rf->we = 1;
    // rf->rd = 2;
    // rf->wd = 0x11;  // Overwrite r2 (was 0xBB)
    // clock_cycle(rf, tfp, time);
    
    // rf->we = 0;
    // rf->rs1 = 2;
    // rf->eval();
    // tfp->dump(time++);
    
    // if (rf->rs1_out != 0x11) {
    //     std::cerr << "  ✗ FAIL: Overwrite failed\n";
    //     return 1;
    // }
    // std::cout << "  ✓ r2 overwritten: 0xBB → 0x11\n\n";
    
    // // Test 7: Read from rd port
    // std::cout << "Test 7: Read from rd port\n";
    // rf->rd = 3;
    // rf->eval();
    // tfp->dump(time++);

    // expected_result = test_values[rf->rd];
    // actual_result = rf->rd_out;
    
    // if (actual_result != expected_result) {
    //     std::cerr << "  ✗ FAIL: rd_out incorrect\n";
    //     return 1;
    // }
    // std::cout << "  ✓ rd[3] = 0x" << std::hex << (int)actual_result << std::dec << "\n\n";
    
    // // Test 8: All registers at once
    // std::cout << "Test 8: Read all three ports simultaneously\n";

    // for (int i = 0; i <= 3; i++) {
    //     rf->we = 1;
    //     rf->rd = i;
    //     rf->wd = test_values[i];
    //     clock_cycle(rf, tfp, time);
    //     std::cout << "  Written r" << i << " = 0b" << std::bitset<8>(test_values[i]) << "\n";
    // }

    // rf->rd = 3;
    // rf->rs1 = 1;
    // rf->rs2 = 2;
    // rf->eval();
    // tfp->dump(time++);
    
    // print_registers(rf, "Triple read:");

    // expected_result_rs1 = test_values[rf->rs1];
    // expected_result_rs2 = test_values[rf->rs2];
    // uint8_t expected_result_rd = test_values[rf->rd];
    // actual_result_rs1 = rf->rs1_out;
    // actual_result_rs2 = rf->rs2_out;
    // uint8_t actual_result_rd = rf->rd_out;
    
    // if (actual_result_rd != expected_result_rd 
    //     || actual_result_rs1 != expected_result_rs1 
    //     || actual_result_rs2 != expected_result_rs2
    // ) {
    //     std::cerr << "  ✗ FAIL: Triple read failed\n";
    //     return 1;
    // }
    // std::cout << "  ✓ All three ports read correctly\n\n";
    
    // Cleanup
    tfp->close();
    delete tfp;
    delete rf;
    
    std::cout << "✅ All tests passed!\n";
    std::cout << "VCD file: waveform_rf.vcd\n";
    return 0;
}
