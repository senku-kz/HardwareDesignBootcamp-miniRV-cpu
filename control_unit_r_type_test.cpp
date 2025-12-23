#include <iostream>
#include <iomanip>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vcontrol_unit.h"

void print_instruction(uint8_t instr) {
    std::cout << "    Instruction: 0b" << std::bitset<8>(instr) 
              << " (0x" << std::hex << (int)instr << std::dec << ")\n";
}

int main(int argc, char** argv) {
    // Initialize Verilator
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);
    
    // Create module and VCD trace
    Vcontrol_unit* cu = new Vcontrol_unit;
    VerilatedVcdC* tfp = new VerilatedVcdC;
    cu->trace(tfp, 99);
    tfp->open("waveform_cu.vcd");
    
    uint64_t time = 0;
    int test_count = 0;

    // Initialize expected values
    uint8_t expected_opcode;
    uint8_t expected_register_destination;
    uint8_t expected_register_source_1;
    uint8_t expected_register_source_2;
    uint8_t expected_function_3;
    uint8_t expected_function_7;

    
    // Initialize real values
    uint8_t real_opcode;
    uint8_t real_register_destination;
    uint8_t real_register_source_1;
    uint8_t real_register_source_2;
    uint8_t real_function_3;
    uint8_t real_function_7;

    
    std::cout << "Testing Control Unit Decoder\n";
    std::cout << "============================\n\n";
    
    // Test 1: ADD-type instruction (opcode = 2'b00)
    std::cout << "Test 1:  R-Type (Register-Register) (OP)\n";
    std::cout << "  Format: [funct7(31:25) | rs2(24:20) | rs1(19:15) | funct3(14:12) | rd(11:7) | opcode(6:0)]\n";
    
    // Instruction: 0000000 | 00000 | 00000 | 000 | 00000 | 01100011 = 0b00000000000000000000000001100011 = 0x00000033
    // funct7=0b0000000, rs2=0b00000, rs1=0b00000, funct3=0b000, rd=0b00000, opcode=0b01100011
    expected_opcode = 0b0110011;
    expected_register_destination = 0b00001;
    expected_register_source_1 = 0b00001;
    expected_register_source_2 = 0b00001;
    expected_function_3 = 0b001;
    expected_function_7 = 0b0000001;

    uint32_t r_type_instr_op = expected_function_7 << 25 | expected_register_source_2 << 20 | expected_register_source_1 << 15 | expected_function_3 << 12 | expected_register_destination << 7 | expected_opcode;

    std::cout << "  Expected: " << std::bitset<32>(r_type_instr_op) << "\n";
    
    cu->instruction = r_type_instr_op;
    cu->eval();
    tfp->dump(time++);

    real_opcode = cu->opcode;
    real_register_destination = cu->register_destination;
    real_register_source_1 = cu->register_source_1;
    real_register_source_2 = cu->register_source_2;
    real_function_3 = cu->function_3;
    real_function_7 = cu->function_7;

    if (real_opcode != expected_opcode) {
        std::cerr << "  FAIL \t opcode=" << std::bitset<7>(real_opcode) << ", expected=" << std::bitset<7>(expected_opcode) << "\n";
        return 1;
    }
    std::cout << "  OK   \t opcode=" << std::bitset<7>(real_opcode) << " (expected " << std::bitset<7>(expected_opcode) << ")\n";

    if (real_register_destination != expected_register_destination) {
        std::cerr << "  FAIL \t register_destination=" << std::bitset<5>(real_register_destination) << ", expected=" << std::bitset<5>(expected_register_destination) << "\n";
        return 1;
    }
    std::cout << "  OK   \t register_destination=" << std::bitset<5>(real_register_destination) << " (expected " << std::bitset<5>(expected_register_destination) << ")\n";

    if (real_register_source_1 != expected_register_source_1) {
        std::cerr << "  FAIL \t register_source_1=" << std::bitset<5>(real_register_source_1) << " (expected " << std::bitset<5>(expected_register_source_1) << ")\n";
        return 1;
    }
    std::cout << "  OK   \t register_source_1=" << std::bitset<5>(real_register_source_1) << " (expected " << std::bitset<5>(expected_register_source_1) << ")\n";

    if (real_register_source_2 != expected_register_source_2) {
        std::cerr << "  FAIL \t register_source_2=" << std::bitset<5>(real_register_source_2) << " (expected " << std::bitset<5>(expected_register_source_2) << ")\n";
        return 1;
    }
    std::cout << "  OK   \t register_source_2=" << std::bitset<5>(real_register_source_2) << " (expected " << std::bitset<5>(expected_register_source_2) << ")\n";
    
    std::cout << "\n";
    
    // Cleanup
    tfp->close();
    delete tfp;
    delete cu;
    
    std::cout << "✅ All " << test_count << " tests passed!\n";
    std::cout << "VCD file: waveform_cu.vcd\n";
    return 0;
}
