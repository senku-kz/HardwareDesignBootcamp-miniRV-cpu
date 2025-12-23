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
    uint16_t expected_immediate_12bit = 0b000000000000000000001;
    uint32_t expected_immediate_32bit = 0x00;

    if (instruction != 0x00) {
        // Extract instruction fields using bit masks and shifts (C++ syntax)
        expected_opcode = (instruction >> 0) & 0x7F;              // bits [6:0]
        expected_register_destination = (instruction >> 7) & 0x1F;  // bits [11:7]
        expected_function_3 = (instruction >> 12) & 0x7;           // bits [14:12]
        expected_register_source_1 = (instruction >> 15) & 0x1F;    // bits [19:15]
        expected_immediate_12bit = (instruction >> 20) & 0xFFF;    // bits [31:20]
        expected_immediate_32bit = (instruction >> 20) & 0xFFF;    // bits [31:20]
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
        return 1;
    }
    std::cout << "  OK   \t register_destination=" << std::bitset<5>(real_register_destination) << " (expected " << std::bitset<5>(expected_register_destination) << ")\n";

    if (real_function_3 != expected_function_3) {
        std::cerr << "  FAIL \t function_3=" << std::bitset<3>(real_function_3) << ", expected=" << std::bitset<3>(expected_function_3) << "\n";
        return 1;
    }
    std::cout << "  OK   \t function_3=" << std::bitset<3>(real_function_3) << " (expected " << std::bitset<3>(expected_function_3) << ")\n";

    if (real_register_source_1 != expected_register_source_1) {
        std::cerr << "  FAIL \t register_source_1=" << std::bitset<5>(real_register_source_1) << " (expected " << std::bitset<5>(expected_register_source_1) << ")\n";
        return 1;
    }
    std::cout << "  OK   \t register_source_1=" << std::bitset<5>(real_register_source_1) << " (expected " << std::bitset<5>(expected_register_source_1) << ")\n";

    if (real_immediate_12bit != expected_immediate_12bit) {
        std::cerr << "  FAIL \t immediate_12bit=" << std::bitset<12>(real_immediate_12bit) << ", expected=" << std::bitset<12>(expected_immediate_12bit) << "\n";
        return 1;
    }
    std::cout << "  OK   \t immediate_12bit=" << std::bitset<12>(real_immediate_12bit) << " (expected " << std::bitset<12>(expected_immediate_12bit) << ")\n";

    if (real_immediate_32bit != expected_immediate_32bit) {
        std::cerr << "  FAIL \t immediate_32bit=" << std::bitset<32>(real_immediate_32bit) << ", expected=" << std::bitset<32>(expected_immediate_32bit) << "\n";
        return 1;
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
    std::cout << "  Format: [immediate(31:25) | rs2(24:20) | rs1(19:15) | funct3(14:12) | opcode(6:0)]\n";

    // S-Type instruction format: [immediate(31:25) | rs2(24:20) | rs1(19:15) | funct3(14:12) | opcode(6:0)]
    // Example: sw x1, 0(x2) -> 0x00002023
    // immediate=0b0000001 (7 bits), rs2=0b00001 (5 bits), rs1=0b00001 (5 bits), funct3=0b000 (3 bits), opcode=0b0100011 (7 bits)
    uint8_t expected_opcode = 0b0100011;
    uint8_t expected_register_source_1 = 0b00001;
    uint8_t expected_register_source_2 = 0b00001;
    uint8_t expected_function_3 = 0b000;
    uint16_t expected_immediate_7bit = 0b000000000000000000001;
    uint32_t expected_immediate_32bit = 0x01;

    if (instruction != 0x00) {
        // Extract instruction fields using bit masks and shifts (C++ syntax)
        expected_opcode = (instruction >> 0) & 0x7F;              // bits [6:0]
        expected_register_source_1 = (instruction >> 7) & 0x1F;    // bits [11:7]
        expected_register_source_2 = (instruction >> 12) & 0x1F;    // bits [19:15]
        expected_function_3 = (instruction >> 15) & 0x7;           // bits [14:12]
        expected_immediate_7bit = (instruction >> 20) & 0x7F;      // bits [31:25]
        expected_immediate_32bit = (instruction >> 20) & 0x7F;    // bits [31:25]
    }

    uint8_t real_opcode = 0x00;
    uint8_t real_register_source_1 = 0x00;
    uint8_t real_register_source_2 = 0x00;
    uint8_t real_function_3 = 0x00;
    uint16_t real_immediate_7bit = 0x00;
    uint32_t real_immediate_32bit = 0x00;
    
    uint32_t s_type_instr_op = expected_immediate_7bit << 25 | expected_register_source_2 << 20 | expected_register_source_1 << 15 | expected_function_3 << 12 | expected_opcode;

    std::cout << "  Expected: " << std::bitset<32>(s_type_instr_op) << "\n";

    cu->instruction = s_type_instr_op;
    cu->eval();
    tfp->dump(time++);

    real_opcode = cu->opcode;
    real_register_source_1 = cu->register_source_1;
    real_register_source_2 = cu->register_source_2;
    real_function_3 = cu->function_3;
    real_immediate_7bit = cu->immediate_7bit;
    real_immediate_32bit = cu->immediate_32bit;

    if (real_opcode != expected_opcode) {
        std::cerr << "  FAIL \t opcode=" << std::bitset<7>(real_opcode) << ", expected=" << std::bitset<7>(expected_opcode) << "\n";
        return 1;
    }
    std::cout << "  OK   \t opcode=" << std::bitset<7>(real_opcode) << " (expected " << std::bitset<7>(expected_opcode) << ")\n";

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

    if (real_function_3 != expected_function_3) {
        std::cerr << "  FAIL \t function_3=" << std::bitset<3>(real_function_3) << ", expected=" << std::bitset<3>(expected_function_3) << "\n";
        return 1;
    }
    std::cout << "  OK   \t function_3=" << std::bitset<3>(real_function_3) << " (expected " << std::bitset<3>(expected_function_3) << ")\n";

    if (real_immediate_7bit != expected_immediate_7bit) {
        std::cerr << "  FAIL \t immediate_7bit=" << std::bitset<7>(real_immediate_7bit) << ", expected=" << std::bitset<7>(expected_immediate_7bit) << "\n";
        return 1;
    }
    std::cout << "  OK   \t immediate_7bit=" << std::bitset<7>(real_immediate_7bit) << " (expected " << std::bitset<7>(expected_immediate_7bit) << ")\n";

    if (real_immediate_32bit != expected_immediate_32bit) {
        std::cerr << "  FAIL \t immediate_32bit=" << std::bitset<32>(real_immediate_32bit) << ", expected=" << std::bitset<32>(expected_immediate_32bit) << "\n";
        return 1;
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
    test_i_type_instruction(cu, tfp, time);

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

    // Test: I-Type (Immediate/Loads/JALR) addi instruction
    test_i_type_instruction(cu, tfp, time, 0x00500093);

    // Test: I-Type (Immediate/Loads/JALR) addi instruction
    test_i_type_instruction(cu, tfp, time, 0x00A08113);

    // Test: I-Type (Immediate/Loads/JALR) addi instruction
    test_i_type_instruction(cu, tfp, time, 0xFFF18193);

    // Test: I-Type (Immediate/Loads/JALR) addi instruction
    test_i_type_instruction(cu, tfp, time, 0x7FF00793);

    // Test: I-Type (Immediate/Loads/JALR) lw instruction
    test_i_type_instruction(cu, tfp, time, 0x0000A103);

    // Test: I-Type (Immediate/Loads/JALR) lw instruction
    test_i_type_instruction(cu, tfp, time, 0x0040A183);

    // Test: I-Type (Immediate/Loads/JALR) lw instruction
    test_i_type_instruction(cu, tfp, time, 0xFFC12203);

    // Test: I-Type (Immediate/Loads/JALR) lbu instruction
    test_i_type_instruction(cu, tfp, time, 0x0000C283);

    // Test: I-Type (Immediate/Loads/JALR) lbu instruction
    test_i_type_instruction(cu, tfp, time, 0x0010C303);

    // Test: I-Type (Immediate/Loads/JALR) lbu instruction
    test_i_type_instruction(cu, tfp, time, 0x0030C383);

    // Test: I-Type (Immediate/Loads/JALR) jalr instruction
    test_i_type_instruction(cu, tfp, time, 0x000280E7);

    // Test: I-Type (Immediate/Loads/JALR) jalr instruction
    test_i_type_instruction(cu, tfp, time, 0x00008067);

    // Test: I-Type (Immediate/Loads/JALR) jalr instruction
    test_i_type_instruction(cu, tfp, time, 0x00810567);


    // Test 3: S-Type (Store)
    test_s_type_instruction(cu, tfp, time);

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

    // Test: S-Type (Store) sw instruction
    test_s_type_instruction(cu, tfp, time, 0x0020A023);

    // Test: S-Type (Store) sw instruction
    test_s_type_instruction(cu, tfp, time, 0x0030A223);

    // Test: S-Type (Store) sw instruction
    test_s_type_instruction(cu, tfp, time, 0x0040A423);
    
    // Test: S-Type (Store) sw instruction
    test_s_type_instruction(cu, tfp, time, 0x0050A623);

    // Test: S-Type (Store) sw instruction
    test_s_type_instruction(cu, tfp, time, 0xFE612E23);

    // Test: S-Type (Store) sb instruction
    test_s_type_instruction(cu, tfp, time, 0x00208023);


    std::cout << "\n";
    
    // Cleanup
    tfp->close();
    delete tfp;
    delete cu;
    
    std::cout << "✅ All " << test_count << " tests passed!\n";
    std::cout << "VCD file: waveform_cu.vcd\n";
    return 0;
}
