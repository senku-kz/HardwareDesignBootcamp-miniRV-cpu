#include <iostream>
#include <iomanip>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vcontrol_unit.h"

uint32_t expected_value = 0x00;
uint32_t real_value = 0x00;



// // Initialize expected values
// uint8_t expected_opcode;
// uint8_t expected_register_destination;
// uint8_t expected_register_source_1;
// uint8_t expected_register_source_2;
// uint8_t expected_function_3;
// uint8_t expected_function_7;
// uint32_t expected_immediate_7bit;
// uint32_t expected_immediate_12bit;
// uint32_t expected_immediate_20bit;
// uint32_t expected_immediate_32bit;


// // Initialize real values
// uint8_t real_opcode;
// uint8_t real_register_destination;
// uint8_t real_register_source_1;
// uint8_t real_register_source_2;
// uint8_t real_function_3;
// uint8_t real_function_7;
// uint32_t real_immediate_7bit;
// uint32_t real_immediate_12bit;
// uint32_t real_immediate_20bit;
// uint32_t real_immediate_32bit;


void print_instruction(uint8_t instr) {
    std::cout << "    Instruction: 0b" << std::bitset<8>(instr) 
              << " (0x" << std::hex << (int)instr << std::dec << ")\n";
}


bool test_r_type_instruction(Vcontrol_unit* cu, VerilatedVcdC* tfp, uint64_t& time, uint32_t instruction = 0x00) {
    std::cout << "Test: R-Type (Register-Register) ";
    if (instruction != 0x00) {
        std::cout << "with instruction 0x" << std::setfill('0') << std::setw(8) << std::hex << instruction << std::dec;
    } else {
        std::cout << "without instruction";
    }
    std::cout << "\n";
    std::cout << "  Format: [funct7(31:25) | rs2(24:20) | rs1(19:15) | funct3(14:12) | rd(11:7) | opcode(6:0)]\n";
    
    // Instruction: 0000000 | 00000 | 00000 | 000 | 00000 | 01100011 = 0b00000000000000000000000001100011 = 0x00000033
    // funct7=0b0000000, rs2=0b00000, rs1=0b00000, funct3=0b000, rd=0b00000, opcode=0b01100011
    uint8_t expected_opcode = 0b0110011;
    uint8_t expected_register_destination = 0b00001;
    uint8_t expected_register_source_1 = 0b00001;
    uint8_t expected_register_source_2 = 0b00001;
    uint8_t expected_function_3 = 0b001;
    uint8_t expected_function_7 = 0b0000001;
    
    if (instruction != 0x00) {
        // Extract instruction fields using bit masks and shifts (C++ syntax)
        expected_opcode = (instruction >> 0) & 0x7F;              // bits [6:0]
        expected_register_destination = (instruction >> 7) & 0x1F;  // bits [11:7]
        expected_function_3 = (instruction >> 12) & 0x7;           // bits [14:12]
        expected_register_source_1 = (instruction >> 15) & 0x1F;    // bits [19:15]
        expected_register_source_2 = (instruction >> 20) & 0x1F;    // bits [24:20]
        expected_function_7 = (instruction >> 25) & 0x7F;           // bits [31:25]
    }

    uint8_t real_opcode = 0x00;
    uint8_t real_register_destination = 0x00;
    uint8_t real_register_source_1 = 0x00;
    uint8_t real_register_source_2 = 0x00;
    uint8_t real_function_3 = 0x00;
    uint8_t real_function_7 = 0x00;

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
        return false;
    }
    std::cout << "  OK   \t opcode=" << std::bitset<7>(real_opcode) << " (expected " << std::bitset<7>(expected_opcode) << ")\n";

    if (real_register_destination != expected_register_destination) {
        std::cerr << "  FAIL \t register_destination=" << std::bitset<5>(real_register_destination) << ", expected=" << std::bitset<5>(expected_register_destination) << "\n";
        return false;
    }
    std::cout << "  OK   \t register_destination=" << std::bitset<5>(real_register_destination) << " (expected " << std::bitset<5>(expected_register_destination) << ")\n";

    if (real_register_source_1 != expected_register_source_1) {
        std::cerr << "  FAIL \t register_source_1=" << std::bitset<5>(real_register_source_1) << " (expected " << std::bitset<5>(expected_register_source_1) << ")\n";
        return false;
    }
    std::cout << "  OK   \t register_source_1=" << std::bitset<5>(real_register_source_1) << " (expected " << std::bitset<5>(expected_register_source_1) << ")\n";

    if (real_register_source_2 != expected_register_source_2) {
        std::cerr << "  FAIL \t register_source_2=" << std::bitset<5>(real_register_source_2) << " (expected " << std::bitset<5>(expected_register_source_2) << ")\n";
        return false;
    }
    std::cout << "  OK   \t register_source_2=" << std::bitset<5>(real_register_source_2) << " (expected " << std::bitset<5>(expected_register_source_2) << ")\n";

    if (real_function_3 != expected_function_3) {
        std::cerr << "  FAIL \t function_3=" << std::bitset<3>(real_function_3) << ", expected=" << std::bitset<3>(expected_function_3) << "\n";
        return false;
    }
    std::cout << "  OK   \t function_3=" << std::bitset<3>(real_function_3) << " (expected " << std::bitset<3>(expected_function_3) << ")\n";

    if (real_function_7 != expected_function_7) {
        std::cerr << "  FAIL \t function_7=" << std::bitset<7>(real_function_7) << ", expected=" << std::bitset<7>(expected_function_7) << "\n";
        return false;
    }
    std::cout << "  OK   \t function_7=" << std::bitset<7>(real_function_7) << " (expected " << std::bitset<7>(expected_function_7) << ")\n";

    std::cout << "\n";
    return true;
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

    
    std::cout << "Testing Control Unit Decoder\n";
    std::cout << "============================\n\n";
    
    // Test 1: R-Type (Register-Register) (OP)
    test_r_type_instruction(cu, tfp, time);

    /*
    add x3, x1, x2 -> 0x002081B3
    add x5, x14, x15 -> 0x00F702B3
    sub x4, x6, x7 -> 0x40730233
    and x8, x9, x10 -> 0x00A4F433
    or x11, x12, x13 -> 0x00D665B3
    xor x2, x2, x3 -> 0x00314133
    sll x1, x1, x2 -> 0x002090B3
    srl x10, x10, x5 -> 0x00555533
    sra x10, x10, x5 -> 0x40555533
    slt x6, x7, x8 -> 0x0083A333
    sltu x6, x7, x8 -> 0x0083B333
    */
    // Test: R-Type (Register-Register) add instruction
    test_r_type_instruction(cu, tfp, time, 0x002081B3);

    // Test: R-Type (Register-Register) add instruction
    test_r_type_instruction(cu, tfp, time, 0x00F702B3);

    // Test: R-Type (Register-Register) add instruction
    test_r_type_instruction(cu, tfp, time, 0x40730233);

    // Test: R-Type (Register-Register) add instruction
    test_r_type_instruction(cu, tfp, time, 0x00A4F433);
    
    
    // Test 2: I-Type (Immediate/Loads/JALR) (JALR)
    // test_i_type_instruction(cu, tfp, time);

    std::cout << "\n";
    
    // Cleanup
    tfp->close();
    delete tfp;
    delete cu;
    
    std::cout << "✅ All " << test_count << " tests passed!\n";
    std::cout << "VCD file: waveform_cu.vcd\n";
    return 0;
}
