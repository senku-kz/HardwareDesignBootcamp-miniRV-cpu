#include <iostream>
#include <iomanip>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vcontrol_unit.h"

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


bool test_i_type_instruction(Vcontrol_unit* cu, VerilatedVcdC* tfp, uint64_t& time, uint32_t instruction = 0x00) {
    std::cout << "Test: I-Type (Immediate/Loads/JALR) ";
    if (instruction != 0x00) {
        std::cout << "with instruction 0x" << std::setfill('0') << std::setw(8) << std::hex << instruction << std::dec;
    } else {
        std::cout << "without instruction";
    }
    std::cout << "\n";
    std::cout << "  Format: [immediate(31:20) | rs1(19:15) | funct3(14:12) | rd(11:7) | opcode(6:0)]\n";

    // I-Type instruction format: [immediate(31:20) | rs1(19:15) | funct3(14:12) | rd(11:7) | opcode(6:0)]
    // Example: addi x1, x1, 1 = 000000000001_00001_000_00001_0010011 = 0x00008093
    // immediate=0b000000000001 (12 bits), rs1=0b00001 (5 bits), funct3=0b000 (3 bits), rd=0b00001 (5 bits), opcode=0b0010011 (7 bits)
    uint8_t expected_opcode = 0b0010011;
    uint8_t expected_register_destination = 0b00001;
    uint8_t expected_register_source_1 = 0b00001;
    uint8_t expected_function_3 = 0b001;
    uint16_t expected_immediate_12bit = 0x001;
    uint8_t sign_bit = 0;
    uint32_t expected_immediate_32bit = expected_immediate_12bit | (sign_bit ? 0xFFFFF000 : 0);

    if (instruction != 0x00) {
        // Extract instruction fields using bit masks and shifts (C++ syntax)
        expected_opcode = (instruction >> 0) & 0x7F;              // bits [6:0]
        expected_register_destination = (instruction >> 7) & 0x1F;  // bits [11:7]
        expected_function_3 = (instruction >> 12) & 0x7;           // bits [14:12]
        expected_register_source_1 = (instruction >> 15) & 0x1F;    // bits [19:15]
        expected_immediate_12bit = (instruction >> 20) & 0xFFF;    // bits [31:20]
        sign_bit = (instruction >> 31) & 1;  // Extract sign bit from bit 31
        expected_immediate_32bit = expected_immediate_12bit | (sign_bit ? 0xFFFFF000 : 0);  // Duplicate sign bit in upper 20 bits
    }

    uint8_t real_opcode = 0x00;
    uint8_t real_register_destination = 0x00;
    uint8_t real_function_3 = 0x00; 
    uint8_t real_register_source_1 = 0x00;
    uint16_t real_immediate_12bit = 0x00;
    uint32_t real_immediate_32bit = 0x00;

    uint32_t i_type_instr_op = expected_immediate_12bit << 20 | expected_register_source_1 << 15 | expected_function_3 << 12 | expected_register_destination << 7 | expected_opcode;

    std::cout << "  Expected: " << std::bitset<32>(i_type_instr_op) << "\n";

    cu->instruction = i_type_instr_op;
    cu->eval();
    tfp->dump(time++);

    real_opcode = cu->opcode;
    real_register_destination = cu->register_destination;
    real_function_3 = cu->function_3;
    real_register_source_1 = cu->register_source_1;
    real_immediate_12bit = cu->immediate_12bit;
    real_immediate_32bit = cu->immediate_32bit;

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

    if (real_function_3 != expected_function_3) {
        std::cerr << "  FAIL \t function_3=" << std::bitset<3>(real_function_3) << ", expected=" << std::bitset<3>(expected_function_3) << "\n";
        return false;
    }
    std::cout << "  OK   \t function_3=" << std::bitset<3>(real_function_3) << " (expected " << std::bitset<3>(expected_function_3) << ")\n";

    if (real_register_source_1 != expected_register_source_1) {
        std::cerr << "  FAIL \t register_source_1=" << std::bitset<5>(real_register_source_1) << " (expected " << std::bitset<5>(expected_register_source_1) << ")\n";
        return false;
    }
    std::cout << "  OK   \t register_source_1=" << std::bitset<5>(real_register_source_1) << " (expected " << std::bitset<5>(expected_register_source_1) << ")\n";

    if (real_immediate_12bit != expected_immediate_12bit) {
        std::cerr << "  FAIL \t immediate_12bit=" << std::bitset<12>(real_immediate_12bit) << ", expected=" << std::bitset<12>(expected_immediate_12bit) << "\n";
        return false;
    }
    std::cout << "  OK   \t immediate_12bit=" << std::bitset<12>(real_immediate_12bit) << " (expected " << std::bitset<12>(expected_immediate_12bit) << ")\n";

    if (real_immediate_32bit != expected_immediate_32bit) {
        std::cerr << "  FAIL \t immediate_32bit=" << std::bitset<32>(real_immediate_32bit) << ", expected=" << std::bitset<32>(expected_immediate_32bit) << "\n";
        return false;
    }
    std::cout << "  OK   \t immediate_32bit=" << std::bitset<32>(real_immediate_32bit) << " (expected " << std::bitset<32>(expected_immediate_32bit) << ")\n";

    std::cout << "\n";
    return true;
}


bool test_s_type_instruction(Vcontrol_unit* cu, VerilatedVcdC* tfp, uint64_t& time, uint32_t instruction = 0x00) {
    std::cout << "Test: S-Type (Store) ";
    if (instruction != 0x00) {
        std::cout << "with instruction 0x" << std::setfill('0') << std::setw(8) << std::hex << instruction << std::dec;
    } else {
        std::cout << "without instruction";
    }
    std::cout << "\n";
    std::cout << "  Format: [immediate(31:25) | rs2(24:20) | rs1(19:15) | funct3(14:12) | immediate(11:7) | opcode(6:0)]\n";

    // S-Type instruction format: [immediate(31:25) | rs2(24:20) | rs1(19:15) | funct3(14:12) | opcode(6:0)]
    // Example: sw x1, 0(x2) -> 0x00002023
    // immediate=0b0000001 (7 bits), rs2=0b00001 (5 bits), rs1=0b00001 (5 bits), funct3=0b000 (3 bits), immediate=0b0000001 (5 bits), opcode=0b0100011 (7 bits)
    uint8_t expected_opcode = 0b0100011;
    uint8_t expected_immediate_5bit = 0b00001;
    uint8_t expected_function_3 = 0b000;
    uint8_t expected_register_source_1 = 0b00001;
    uint8_t expected_register_source_2 = 0b00001;
    uint8_t expected_immediate_7bit = 0b0000001;
    uint8_t sign_bit = 0;
    uint32_t expected_immediate_32bit = (sign_bit ? 0xFFFFF800 : 0) | expected_immediate_7bit << 5 | expected_immediate_5bit;

    if (instruction != 0x00) {
        // Extract instruction fields using bit masks and shifts (C++ syntax)
        expected_opcode = (instruction >> 0) & 0x7F;              // bits [6:0]
        expected_immediate_5bit = (instruction >> 7) & 0x1F;    // bits [11:7]
        expected_function_3 = (instruction >> 12) & 0x7;           // bits [14:12]
        expected_register_source_1 = (instruction >> 15) & 0x1F;    // bits [19:15]
        expected_register_source_2 = (instruction >> 20) & 0x1F;    // bits [24:20]
        expected_immediate_7bit = (instruction >> 25) & 0x7F;      // bits [31:25]
        sign_bit = (instruction >> 31) & 1;  // Extract sign bit from bit 31
        expected_immediate_32bit = (sign_bit ? 0xFFFFF800 : 0) | expected_immediate_7bit << 5 | expected_immediate_5bit;
    }

    uint8_t real_opcode = 0x00;
    uint8_t real_immediate_5bit = 0x00;
    uint8_t real_function_3 = 0x00;
    uint8_t real_register_source_1 = 0x00;
    uint8_t real_register_source_2 = 0x00;
    uint8_t real_immediate_7bit = 0x00;
    uint32_t real_immediate_32bit = 0x00;
    
    uint32_t s_type_instr_op = expected_immediate_7bit << 25 
    | expected_register_source_2 << 20 
    | expected_register_source_1 << 15 
    | expected_function_3 << 12 
    | expected_immediate_5bit << 7
    | expected_opcode;

    std::cout << "  Expected: " << std::bitset<32>(s_type_instr_op) << "\n";

    cu->instruction = s_type_instr_op;
    cu->eval();
    tfp->dump(time++);

    real_opcode = cu->opcode;
    real_immediate_5bit = cu->register_destination;
    real_function_3 = cu->function_3;
    real_register_source_1 = cu->register_source_1;
    real_register_source_2 = cu->register_source_2;
    real_immediate_7bit = cu->immediate_7bit;
    real_immediate_32bit = cu->immediate_32bit;

    if (real_opcode != expected_opcode) {
        std::cerr << "  FAIL \t opcode=" << std::bitset<7>(real_opcode) << ", expected=" << std::bitset<7>(expected_opcode) << "\n";
        return false;
    }
    std::cout << "  OK   \t opcode=" << std::bitset<7>(real_opcode) << " (expected " << std::bitset<7>(expected_opcode) << ")\n";

    if (real_immediate_5bit != expected_immediate_5bit) {
        std::cerr << "  FAIL \t immediate_5bit=" << std::bitset<5>(real_immediate_5bit) << ", expected=" << std::bitset<5>(expected_immediate_5bit) << "\n";
        return false;
    }
    std::cout << "  OK   \t immediate_5bit=" << std::bitset<5>(real_immediate_5bit) << " (expected " << std::bitset<5>(expected_immediate_5bit) << ")\n";

    if (real_function_3 != expected_function_3) {
        std::cerr << "  FAIL \t function_3=" << std::bitset<3>(real_function_3) << ", expected=" << std::bitset<3>(expected_function_3) << "\n";
        return false;
    }
    std::cout << "  OK   \t function_3=" << std::bitset<3>(real_function_3) << " (expected " << std::bitset<3>(expected_function_3) << ")\n";

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

    if (real_immediate_7bit != expected_immediate_7bit) {
        std::cerr << "  FAIL \t immediate_7bit=" << std::bitset<7>(real_immediate_7bit) << ", expected=" << std::bitset<7>(expected_immediate_7bit) << "\n";
        return false;
    }
    std::cout << "  OK   \t immediate_7bit=" << std::bitset<7>(real_immediate_7bit) << " (expected " << std::bitset<7>(expected_immediate_7bit) << ")\n";

    if (real_immediate_32bit != expected_immediate_32bit) {
        std::cerr << "  FAIL \t immediate_32bit=" << std::bitset<32>(real_immediate_32bit) << ", expected=" << std::bitset<32>(expected_immediate_32bit) << "\n";
        return false;
    }
    std::cout << "  OK   \t immediate_32bit=" << std::bitset<32>(real_immediate_32bit) << " (expected " << std::bitset<32>(expected_immediate_32bit) << ")\n";

    std::cout << "\n";
    return true;
}


bool test_u_type_instruction(Vcontrol_unit* cu, VerilatedVcdC* tfp, uint64_t& time, uint32_t instruction = 0x00) {
    std::cout << "Test: U-Type (LUI/AUIPC) ";
    if (instruction != 0x00) {
        std::cout << "with instruction 0x" << std::setfill('0') << std::setw(8) << std::hex << instruction << std::dec;
    } else {
        std::cout << "without instruction";
    }
    std::cout << "\n";
    std::cout << "  Format: [immediate(31:12) | rd(11:7) | opcode(6:0)]\n";

    // U-Type instruction format: [immediate(31:12) | rd(11:7) | opcode(6:0)]
    // Example: lui x1, 1024 -> 0x00004037
    // immediate=0b00000000000000000000 (20 bits), rd=0b00001 (5 bits), opcode=0b0110111 (7 bits)
    uint8_t expected_opcode = 0b0110111;
    uint8_t expected_register_destination = 0b00001;
    uint32_t expected_immediate_20bit = 0x00001;
    uint32_t expected_immediate_32bit = expected_immediate_20bit << 12;

    if (instruction != 0x00) {
        // Extract instruction fields using bit masks and shifts (C++ syntax)
        expected_opcode = (instruction >> 0) & 0x7F;              // bits [6:0]
        expected_register_destination = (instruction >> 7) & 0x1F;  // bits [11:7]
        expected_immediate_20bit = (instruction >> 12) & 0xFFFFF;    // bits [31:12]
        expected_immediate_32bit = expected_immediate_20bit << 12;    // bits [31:12]
    }

    uint8_t real_opcode = 0x00;
    uint8_t real_register_destination = 0x00;
    uint32_t real_immediate_20bit = 0x00;
    uint32_t real_immediate_32bit = 0x00;
    
    uint32_t u_type_instr_op = expected_immediate_20bit << 12 | expected_register_destination << 7 | expected_opcode;

    std::cout << "  Expected: " << std::bitset<32>(u_type_instr_op) << "\n";

    cu->instruction = u_type_instr_op;
    cu->eval();
    tfp->dump(time++);

    real_opcode = cu->opcode;
    real_register_destination = cu->register_destination;
    real_immediate_20bit = cu->immediate_20bit;
    real_immediate_32bit = cu->immediate_32bit;

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

    if (real_immediate_20bit != expected_immediate_20bit) { 
        std::cerr << "  FAIL \t immediate_20bit=" << std::bitset<20>(real_immediate_20bit) << ", expected=" << std::bitset<20>(expected_immediate_20bit) << "\n";
        return false;
    }
    std::cout << "  OK   \t immediate_20bit=" << std::bitset<20>(real_immediate_20bit) << " (expected " << std::bitset<20>(expected_immediate_20bit) << ")\n";

    if (real_immediate_32bit != expected_immediate_32bit) {
        std::cerr << "  FAIL \t immediate_32bit=" << std::bitset<32>(real_immediate_32bit) << ", expected=" << std::bitset<32>(expected_immediate_32bit) << "\n";
        return false;
    }
    std::cout << "  OK   \t immediate_32bit=" << std::bitset<32>(real_immediate_32bit) << " (expected " << std::bitset<32>(expected_immediate_32bit) << ")\n";

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
    bool test_result = false;
    int test_count = 0;
    int test_success = 0;

    
    std::cout << "Testing Control Unit Decoder\n";
    std::cout << "============================\n\n";
    
    // Test 1: R-Type (Register-Register) (OP)
    test_result = test_r_type_instruction(cu, tfp, time);
    if (test_result) {
        test_success++;
    }
    test_count++;

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
    uint32_t test_r_instructions[] = {0x002081B3, 0x00F702B3, 0x40730233, 0x00A4F433, 0x00D665B3, 0x00314133, 0x002090B3, 0x00555533, 0x40555533, 0x0083A333, 0x0083B333};
    for (int i = 0; i < sizeof(test_r_instructions) / sizeof(test_r_instructions[0]); i++) {
        test_result = test_r_type_instruction(cu, tfp, time, test_r_instructions[i]);
        if (test_result) {
            test_success++;
        }
        test_count++;
    }

    
    
    // Test 2: I-Type (Immediate/Loads/JALR) (JALR)
    test_result = test_i_type_instruction(cu, tfp, time);
    if (test_result) {
        test_success++;
    }
    test_count++;

    /*
    ADDI (opcode 0x13)
    addi x1, x0, 5 -> 0x00500093
    addi x2, x1, 10 -> 0x00A08113
    addi x3, x3, -1 -> 0xFFF18193
    addi x15, x0, 2047 -> 0x7FF00793
    LW (opcode 0x03, funct3=010)
    5) lw x2, 0(x1) -> 0x0000A103
    6) lw x3, 4(x1) -> 0x0040A183
    7) lw x4, -4(x2) -> 0xFFC12203
    LBU (opcode 0x03, funct3=100)
    8) lbu x5, 0(x1) -> 0x0000C283
    9) lbu x6, 1(x1) -> 0x0010C303
    10) lbu x7, 3(x1) -> 0x0030C383
    JALR (opcode 0x67, funct3=000)
    11) jalr x1, 0(x5) -> 0x000280E7
    12) jalr x0, 0(x1) -> 0x00008067
    13) jalr x10, 8(x2) -> 0x00810567
    */
    uint32_t test_i_instructions[] = {0x00500093, 0x00A08113, 0xFFF18193, 0x7FF00793, 0x0000A103, 0x0040A183, 0xFFC12203, 0x0000C283, 0x0010C303, 0x0030C383, 0x000280E7, 0x00008067, 0x00810567};
    for (int i = 0; i < sizeof(test_i_instructions) / sizeof(test_i_instructions[0]); i++) {
        test_result = test_i_type_instruction(cu, tfp, time, test_i_instructions[i]);
        if (test_result) {
            test_success++;
        }
        test_count++;
    }


    // Test 3: S-Type (Store)
    test_result = test_s_type_instruction(cu, tfp, time);
    if (test_result) {
        test_success++;
    }
    test_count++;

    /*
    sw x2, 0(x1)    -> 0x0020A023
    sw x3, 4(x1)    -> 0x0030A223
    sw x4, 8(x1)    -> 0x0040A423
    sw x5, 12(x1)   -> 0x0050A623
    sw x6, -4(x2)   -> 0xFE612E23

    sb x2, 0(x1)    -> 0x00208023
    sb x3, 1(x1)    -> 0x003080A3
    sb x4, 2(x1)    -> 0x00408123
    sb x5, 3(x1)    -> 0x005081A3
    sb x6, -1(x2)   -> 0xFE610FA3
    */
    uint32_t test_s_instructions[] = {0x0020A023, 0x0030A223, 0x0040A423, 0x0050A623, 0xFE612E23, 0x00208023, 0x003080A3, 0x00408123, 0x005081A3, 0xFE610FA3};
    for (int i = 0; i < sizeof(test_s_instructions) / sizeof(test_s_instructions[0]); i++) {
        test_result = test_s_type_instruction(cu, tfp, time, test_s_instructions[i]);
        if (test_result) {
            test_success++;
        }
        test_count++;
    }


    // Test 4: U-Type
    test_result = test_u_type_instruction(cu, tfp, time);
    if (test_result) {
        test_success++;
    }
    test_count++;

    /*
    lui x1, 1024 -> 0x00004037
    lui x2, 2048 -> 0x00008037
    lui x3, 4096 -> 0x00010037
    lui x4, 8192 -> 0x00020037
    lui x5, 16384 -> 0x00040037
    */

    uint32_t test_u_instructions[] = {0x00004037, 0x00008037, 0x00010037, 0x00020037, 0x00040037};
    for (int i = 0; i < sizeof(test_u_instructions) / sizeof(test_u_instructions[0]); i++) {
        test_result = test_u_type_instruction(cu, tfp, time, test_u_instructions[i]);
        if (test_result) {
            test_success++;
        }
        test_count++;
    }
    std::cout << "\n";
    
    // Cleanup
    tfp->close();
    delete tfp;
    delete cu;
    


    if (test_success == test_count) {
        std::cout << "✅ All " << test_success << " tests passed!\n";
    } else {
        std::cout << "❌ " << test_count - test_success << " tests failed!\n";
    }
    std::cout << "VCD file: waveform_cu.vcd\n";
    return 0;
}
