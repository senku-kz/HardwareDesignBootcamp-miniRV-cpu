#include <iostream>
#include <iomanip>
#include <bitset>
#include <random>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vwriteback_mux.h"


uint32_t generate_random_32bit() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<uint32_t> dis(0, UINT32_MAX);
    return dis(gen);
}


bool test_writeback_selection(Vwriteback_mux* wb_mux, VerilatedVcdC* tfp, uint64_t& time
    , uint8_t wb_sel, uint32_t alu_result, uint32_t mem_rdata, uint32_t pc_plus4, uint32_t imm_u, uint32_t expected_wb_data) {
    std::cout << "Test: Writeback Selection\n";

    wb_mux->alu_result = alu_result;
    wb_mux->mem_rdata = mem_rdata;
    wb_mux->pc_plus4 = pc_plus4;
    wb_mux->imm_u = imm_u;
    wb_mux->wb_sel = wb_sel;

    wb_mux->eval();
    tfp->dump(time++);

    uint32_t real_wb_data = wb_mux->wb_data;
    
    if (real_wb_data != expected_wb_data) {
        std::cerr << "  FAIL \t wb_data = 0x" << std::setfill('0') << std::setw(8) << std::hex << real_wb_data 
                  << ", expected = 0x" << std::setfill('0') << std::setw(8) << std::hex << expected_wb_data << std::dec << "\n";
        return false;
    }
    std::cout << "  OK   \t wb_data = 0x" << std::setfill('0') << std::setw(8) << std::hex << real_wb_data 
              << " (expected 0x" << std::setfill('0') << std::setw(8) << std::hex << expected_wb_data << ")" << std::dec << "\n";
    std::cout << "\n";
    return true;
}


bool test_wb_sel_alu_result(Vwriteback_mux* wb_mux, VerilatedVcdC* tfp, uint64_t& time) {
    std::cout << "Test: Writeback Selection ALU Result (wb_sel = 00)\n";
    std::cout << "  Format: wb_sel[1:0] = 2'b00 -> wb_data = alu_result\n";
    
    uint32_t alu_result;
    uint32_t mem_rdata;
    uint32_t pc_plus4;
    uint32_t imm_u;
    uint8_t wb_sel = 0;
    uint32_t expected_wb_data;

    bool all_passed = true;
    
    for (int i = 0; i < 10; i++) {
        alu_result = generate_random_32bit();
        mem_rdata = generate_random_32bit();
        pc_plus4 = generate_random_32bit();
        imm_u = generate_random_32bit();

        expected_wb_data = alu_result;
        all_passed = all_passed && test_writeback_selection(wb_mux, tfp, time, wb_sel, alu_result, mem_rdata, pc_plus4, imm_u, expected_wb_data);
    }
    return all_passed;
}


bool test_wb_sel_mem_rdata(Vwriteback_mux* wb_mux, VerilatedVcdC* tfp, uint64_t& time) {
    std::cout << "Test: Writeback Selection Memory Read Data (wb_sel = 01)\n";
    std::cout << "  Format: wb_sel[1:0] = 2'b01 -> wb_data = mem_rdata\n";
        
    uint32_t alu_result;
    uint32_t mem_rdata;
    uint32_t pc_plus4;
    uint32_t imm_u;
    uint8_t wb_sel = 1;
    uint32_t expected_wb_data;

    bool all_passed = true;

    for (int i = 0; i < 10; i++) {
        alu_result = generate_random_32bit();
        mem_rdata = generate_random_32bit();
        pc_plus4 = generate_random_32bit();
        imm_u = generate_random_32bit();

        expected_wb_data = mem_rdata;
        all_passed = all_passed && test_writeback_selection(wb_mux, tfp, time, wb_sel, alu_result, mem_rdata, pc_plus4, imm_u, expected_wb_data);
    }
    return all_passed;
}


bool test_wb_sel_pc_plus4(Vwriteback_mux* wb_mux, VerilatedVcdC* tfp, uint64_t& time) {
    std::cout << "Test: Writeback Selection PC+4 (wb_sel = 10)\n";
    std::cout << "  Format: wb_sel[1:0] = 2'b10 -> wb_data = pc_plus4\n";
    
    uint32_t alu_result;
    uint32_t mem_rdata;
    uint32_t pc_plus4;
    uint32_t imm_u;
    uint8_t wb_sel = 2;
    uint32_t expected_wb_data;

    bool all_passed = true;
    
    for (int i = 0; i < 10; i++) {
        alu_result = generate_random_32bit();
        mem_rdata = generate_random_32bit();
        pc_plus4 = generate_random_32bit();
        imm_u = generate_random_32bit();

        expected_wb_data = pc_plus4;
        all_passed = all_passed && test_writeback_selection(wb_mux, tfp, time, wb_sel, alu_result, mem_rdata, pc_plus4, imm_u, expected_wb_data);
    }
    return all_passed;
}


bool test_wb_sel_imm_u(Vwriteback_mux* wb_mux, VerilatedVcdC* tfp, uint64_t& time) {
    std::cout << "Test: Writeback Selection Immediate U-Type (wb_sel = 11)\n";
    std::cout << "  Format: wb_sel[1:0] = 2'b11 -> wb_data = imm_u\n";
    
    uint32_t alu_result;
    uint32_t mem_rdata;
    uint32_t pc_plus4;
    uint32_t imm_u;
    uint8_t wb_sel = 3;
    uint32_t expected_wb_data;

    bool all_passed = true;
    
    for (int i = 0; i < 10; i++) {
        alu_result = generate_random_32bit();
        mem_rdata = generate_random_32bit();
        pc_plus4 = generate_random_32bit();
        imm_u = generate_random_32bit();

        expected_wb_data = imm_u;
        all_passed = all_passed && test_writeback_selection(wb_mux, tfp, time, wb_sel, alu_result, mem_rdata, pc_plus4, imm_u, expected_wb_data);
    }
    return all_passed;
}


bool test_all_selections_comprehensive(Vwriteback_mux* wb_mux, VerilatedVcdC* tfp, uint64_t& time) {
    std::cout << "Test: Comprehensive Writeback Selection Test\n";
    std::cout << "  Testing all selection values with various input combinations\n";
    
    // Test case 1: All inputs are different and easily distinguishable
    struct TestCase {
        uint8_t wb_sel;
        uint32_t alu_result;
        uint32_t mem_rdata;
        uint32_t pc_plus4;
        uint32_t imm_u;
        uint32_t expected_wb_data;
        const char* description;
    };
    
    TestCase test_cases[] = {
        {0, 0x00000001, 0x00000002, 0x00000003, 0x00000004, 0x00000001, "Select ALU result (0x00000001)"},
        {1, 0x00000001, 0x00000002, 0x00000003, 0x00000004, 0x00000002, "Select memory read data (0x00000002)"},
        {2, 0x00000001, 0x00000002, 0x00000003, 0x00000004, 0x00000003, "Select PC+4 (0x00000003)"},
        {3, 0x00000001, 0x00000002, 0x00000003, 0x00000004, 0x00000004, "Select immediate U-type (0x00000004)"},
        {0, 0xFFFFFFFF, 0x00000000, 0x12345678, 0xABCDEF00, 0xFFFFFFFF, "Select ALU result (0xFFFFFFFF)"},
        {1, 0xFFFFFFFF, 0x12345678, 0x00000000, 0xABCDEF00, 0x12345678, "Select memory read data (0x12345678)"},
        {2, 0xFFFFFFFF, 0x12345678, 0xABCDEF00, 0x00000000, 0xABCDEF00, "Select PC+4 (0xABCDEF00)"},
        {3, 0xFFFFFFFF, 0x12345678, 0xABCDEF00, 0xDEADBEEF, 0xDEADBEEF, "Select immediate U-type (0xDEADBEEF)"},
    };
    
    bool all_passed = true;
    for (size_t i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
        std::cout << "  Test case " << (i + 1) << ": " << test_cases[i].description << "\n";
        all_passed = all_passed && test_writeback_selection(
            wb_mux, tfp, time,
            test_cases[i].wb_sel,
            test_cases[i].alu_result,
            test_cases[i].mem_rdata,
            test_cases[i].pc_plus4,
            test_cases[i].imm_u,
            test_cases[i].expected_wb_data
        );
    }
    
    return all_passed;
}


int main(int argc, char** argv) {
    // Initialize Verilator
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);
    
    // Create module and VCD trace
    Vwriteback_mux* wb_mux = new Vwriteback_mux;
    VerilatedVcdC* tfp = new VerilatedVcdC;
    wb_mux->trace(tfp, 99);
    tfp->open("waveform_writeback_mux.vcd");
    
    uint64_t time = 0;
    bool test_result = false;
    int test_count = 0;
    int test_success = 0;
    
    std::cout << "Testing Writeback Multiplexer\n";
    std::cout << "=============================\n\n";
    
    // Test 1: wb_sel = 0 (ALU result)
    test_result = test_wb_sel_alu_result(wb_mux, tfp, time);
    if (test_result) {
        test_success++;
    }
    test_count++;
    
    // Test 2: wb_sel = 1 (Memory read data)
    test_result = test_wb_sel_mem_rdata(wb_mux, tfp, time);
    if (test_result) {
        test_success++;
    }
    test_count++;
    
    // Test 3: wb_sel = 2 (PC+4)
    test_result = test_wb_sel_pc_plus4(wb_mux, tfp, time);
    if (test_result) {
        test_success++;
    }
    test_count++;
    
    // Test 4: wb_sel = 3 (Immediate U-type)
    test_result = test_wb_sel_imm_u(wb_mux, tfp, time);
    if (test_result) {
        test_success++;
    }
    test_count++;
    
    // Test 5: Comprehensive test with multiple input combinations
    test_result = test_all_selections_comprehensive(wb_mux, tfp, time);
    if (test_result) {
        test_success++;
    }
    test_count++;
    
    std::cout << "\n";
    
    // Cleanup
    tfp->close();
    delete tfp;
    delete wb_mux;
    
    if (test_success == test_count) {
        std::cout << "✅ All " << test_success << " tests passed!\n";
    } else {
        std::cout << "❌ " << test_count - test_success << " tests failed!\n";
    }
    std::cout << "VCD file: waveform_writeback_mux.vcd\n";
    return (test_success == test_count) ? 0 : 1;
}
