#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vinstruction_fetch.h"

const std::string TEST_PROGRAM_FILE = "logisim-bin/test-pc4.hex";
int MEMORY_SIZE = 16;


uint32_t expected_value = 0x00;
uint32_t real_value = 0x00;


void read_hex_file(std::string filename, uint32_t* memory) {
    std::ifstream file(filename);
    std::string line;
    
    for (int i = 0; i < MEMORY_SIZE; i++) {
        memory[i] = 0;
    }

    int i = 0;
    while (std::getline(file, line) && i < MEMORY_SIZE) {
        memory[i] = std::stoul(line, nullptr, 16);
        i++;
    }
}


bool test_read_all_initialized_instructions(Vinstruction_fetch* rom, VerilatedVcdC* tfp, uint64_t& time) {
    uint32_t memory[MEMORY_SIZE];
    read_hex_file(TEST_PROGRAM_FILE, memory);

    for (int addr = 0; addr < MEMORY_SIZE; addr++) {
        rom->address = addr << 2;
        rom->eval();
        tfp->dump(time++);

        expected_value = memory[addr];
        real_value = rom->instruction;

        if (real_value != expected_value) {
            std::cerr << "FAIL: Initialized instructions failed (expected 0x" << std::setfill('0') << std::setw(8) << std::hex << expected_value 
            << ", got 0x" << std::setfill('0') << std::setw(8) << std::hex << real_value << " at address " 
            << std::setfill('0') << std::setw(2) << std::dec << addr << ")\n" << std::dec;
            return false;
        }
        // std::cout << "\t ok \t Address " << std::setfill('0') << std::setw(2) << std::dec << addr 
        //           << ": 0x" << std::setfill('0') << std::setw(8) << std::hex << real_value 
        //           << "\t (expected 0x" << std::setfill('0') << std::setw(8) << std::hex << expected_value << ")\n" << std::dec;
    }
    std::cout << "\t ok \t All instructions are read correctly\n";
    return true;
}


bool test_immediate_response(Vinstruction_fetch* rom, VerilatedVcdC* tfp, uint64_t& time) {
    uint32_t memory[MEMORY_SIZE];
    read_hex_file(TEST_PROGRAM_FILE, memory);

    int test_sequence[7] = {3, 0, 4, 1, 5, 2, 1};

    for (int i = 0; i < sizeof(test_sequence) / sizeof(test_sequence[0]); i++) {
        rom->address = test_sequence[i] << 2;
        rom->eval();
        tfp->dump(time++);
        
        expected_value = memory[test_sequence[i]];
        real_value = rom->instruction;

        if (real_value != expected_value) {
            std::cerr << "  Error \t Expected 0x" << std::setfill('0') << std::setw(8) << std::hex << expected_value 
                      << ", got 0x" << std::setfill('0') << std::setw(8) << std::hex << real_value << std::dec << "\n";
            return false;
        }
        std::cout << "\t ok \t Address " << std::setfill('0') << std::setw(2) << std::dec << test_sequence[i] 
                  << ": 0x" << std::setfill('0') << std::setw(8) << std::hex << real_value 
                  << "\t (expected 0x" << std::setfill('0') << std::setw(8) << std::hex << expected_value << ")\n" << std::dec;
    }
    std::cout << "\t ok \t instruction = " << expected_value << "\n";
    return true;
}


bool test_full_address_space_coverage(Vinstruction_fetch* rom, VerilatedVcdC* tfp, uint64_t& time) {
    uint32_t memory[MEMORY_SIZE];
    read_hex_file(TEST_PROGRAM_FILE, memory);

    for (int addr = 0; addr < MEMORY_SIZE; addr++) {
        rom->address = addr << 2;
        rom->eval();
        tfp->dump(time++);
        
        expected_value = memory[addr];
        real_value = rom->instruction;
        if (real_value != expected_value) {
            std::cerr << "FAIL: Full address space coverage failed (expected " << expected_value << ", got " << real_value << ")\n";
            return false;
        }
    }
    std::cout << "\t ok \t All addresses are readable\n";
    return true;
}

int main(int argc, char** argv) {
    // Initialize Verilator
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);
    
    // Create module and VCD trace
    Vinstruction_fetch* instruction_fetch = new Vinstruction_fetch;
    VerilatedVcdC* tfp = new VerilatedVcdC;
    instruction_fetch->trace(tfp, 99);
    tfp->open("waveform_rom.vcd");
    
    uint64_t time = 0;
    
    std::cout << "Testing Instruction ROM (Combinational Logic)\n";
    std::cout << "============================================\n\n";
    
    // Test 1: Read all initialized instructions
    std::cout << "Test 1: Read all initialized instructions\n";
    test_read_all_initialized_instructions(instruction_fetch, tfp, time);
    
    // Test 2: Combinational property - immediate response to address change
    std::cout << "\nTest 2: Combinational logic (immediate response)\n";
    test_immediate_response(instruction_fetch, tfp, time);

    
    // Test 3: All addresses are readable
    std::cout << "\nTest 3: Full address space coverage\n";
    test_full_address_space_coverage(instruction_fetch, tfp, time);

    
    // Cleanup
    tfp->close();
    delete tfp;
    delete instruction_fetch;
    
    std::cout << "\nAll tests passed!\n";
    std::cout << "VCD file: waveform_instruction_fetch.vcd\n";
    return 0;
}
