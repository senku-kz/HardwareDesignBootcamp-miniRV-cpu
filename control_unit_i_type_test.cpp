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
    uint8_t expected_function_3;
    uint8_t expected_function_7;
    uint8_t expected_immediate_12bit;
    
    // Initialize real values
    uint8_t real_opcode;
    uint8_t real_register_destination;
    uint8_t real_register_source_1;
    uint8_t real_function_3;
    uint8_t real_function_7;
    uint8_t real_immediate_12bit;

    
    std::cout << "Testing Control Unit Decoder\n";
    std::cout << "============================\n\n";
    
    // Test 2.1: ADD-type instruction (opcode = 2'b0010011)
    std::cout << "Test 2.1:  I-Type (Immediate/Loads/JALR) (OP-IMM) (ADD-type / ADDI-type)\n";
    std::cout << "  Format: [imm(31:20) | rs1(19:15) | funct3(14:12) | rd(11:7) | opcode(6:0)]\n";
    
    // Instruction: 
    expected_opcode = 0b0010011;
    expected_register_destination = 0b00001;
    expected_register_source_1 = 0b00001;
    expected_function_3 = 0b001;
    expected_immediate_12bit = 0b000000000000000000001;

    uint32_t i_type_instr_addi = expected_immediate_12bit << 20 | expected_register_source_1 << 15 | expected_function_3 << 12 | expected_register_destination << 7 | expected_opcode;

    cu->instruction = i_type_instr_addi;
    cu->eval();
    tfp->dump(time++);

    real_opcode = cu->opcode;
    real_register_destination = cu->register_destination;
    real_register_source_1 = cu->register_source_1;
    real_function_3 = cu->function_3;
    real_immediate_12bit = cu->immediate_12bit;

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
        std::cerr << "  FAIL \t immediate_12bit=" << std::bitset<11>(real_immediate_12bit) << ", expected=" << std::bitset<11>(expected_immediate_12bit) << "\n";
        return 1;
    }
    std::cout << "  OK   \t immediate_12bit=" << std::bitset<11>(real_immediate_12bit) << " (expected " << std::bitset<11>(expected_immediate_12bit) << ")\n";
    
    std::cout << "\n";
    

    // Test 2.2: JALR instruction (opcode = 2'b1100111)
    std::cout << "Test 2.2:  I-Type (Immediate/Loads/JALR) (JALR)\n";
    std::cout << "  Format: [imm(31:20) | rs1(19:15) | funct3(14:12) | rd(11:7) | opcode(6:0)]\n";
    
    // Instruction: 
    expected_opcode = 0b1100111;
    expected_register_destination = 0b00001;
    expected_register_source_1 = 0b00001;
    expected_function_3 = 0b001;
    expected_immediate_12bit = 0b000000000000000000001;

    uint32_t i_type_instr_jalr = expected_immediate_12bit << 20 | expected_register_source_1 << 15 | expected_function_3 << 12 | expected_register_destination << 7 | expected_opcode;

    cu->instruction = i_type_instr_jalr;
    cu->eval();
    tfp->dump(time++);

    real_opcode = cu->opcode;
    real_register_destination = cu->register_destination;
    real_register_source_1 = cu->register_source_1;
    real_function_3 = cu->function_3;
    real_immediate_12bit = cu->immediate_12bit;

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
        std::cerr << "  FAIL \t immediate_12bit=" << std::bitset<20>(real_immediate_12bit) << ", expected=" << std::bitset<20>(expected_immediate_12bit) << "\n";
        return 1;
    }
    std::cout << "  OK   \t immediate_12bit=" << std::bitset<20>(real_immediate_12bit) << " (expected " << std::bitset<20>(expected_immediate_12bit) << ")\n";
    
    std::cout << "\n";


    // Cleanup
    tfp->close();
    delete tfp;
    delete cu;
    
    std::cout << "✅ All " << test_count << " tests passed!\n";
    std::cout << "VCD file: waveform_cu.vcd\n";
    return 0;
}
