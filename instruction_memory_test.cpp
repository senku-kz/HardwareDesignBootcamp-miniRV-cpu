#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vinstruction_memory.h"

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
    while (std::getline(file, line)) {
        memory[i] = std::stoul(line, nullptr, 16);
        i++;
    }
}


int main(int argc, char** argv) {
    // Инициализация Verilator
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);
    
    // Создание модуля и VCD trace
    Vinstruction_memory* rom = new Vinstruction_memory;
    VerilatedVcdC* tfp = new VerilatedVcdC;
    rom->trace(tfp, 99);
    tfp->open("waveform_rom.vcd");
    
    uint64_t time = 0;
    
    std::cout << "Testing Instruction ROM (Combinational Logic)\n";
    std::cout << "============================================\n\n";
    
    // Test 1: Read all initialized instructions
    std::cout << "Test 1: Read all initialized instructions\n";
    uint32_t memory[MEMORY_SIZE];
    read_hex_file("logisim-bin/test.hex", memory);
    uint32_t expected;
    uint32_t actual;

    // for (int i = 0; i < 16; i++) {
    //     std::cout << "  Memory[" << i << "] = 0x" << std::hex << memory[i] << "\n";
    // }
    
    for (int addr = 0; addr < MEMORY_SIZE; addr++) {
        rom->address = addr;
        rom->eval();
        tfp->dump(time++);
        
        expected = memory[addr];
        actual = rom->instruction;
        
        if (actual != expected) {
            std::cerr << "  Error \t Expected 0x" << std::setfill('0') << std::setw(8) << std::hex << expected 
                      << ", got 0x" << std::setfill('0') << std::setw(8) << std::hex << actual << std::dec << "\n";
            return 1;
        }
        std::cout << "  ok \t Address " << std::setfill('0') << std::setw(2) << std::dec << addr 
                  << ": 0x" << std::setfill('0') << std::setw(8) << std::hex << actual 
                  << "\t (expected 0x" << std::setfill('0') << std::setw(8) << std::hex << expected << ")\n" << std::dec;
    }
    
    // Test 2: Read zero-initialized addresses
    std::cout << "\nTest 2: Read zero-initialized addresses (6-15)\n";
    for (int addr = 6; addr < MEMORY_SIZE; addr++) {
        rom->address = addr;
        rom->eval();
        tfp->dump(time++);

        expected = 0;
        actual = rom->instruction;
        if (actual != expected) {
            std::cerr << "  Error \t Expected 0x" << std::setfill('0') << std::setw(8) << std::hex << expected 
                      << ", got 0x" << std::setfill('0') << std::setw(8) << std::hex << actual << std::dec << "\n";
            return 1;
        }
        std::cout << "  ok \t Address " << std::setfill('0') << std::setw(2) << std::dec << addr 
                  << ": 0x" << std::setfill('0') << std::setw(8) << std::hex << actual 
                  << "\t (expected 0x" << std::setfill('0') << std::setw(8) << std::hex << expected << ")\n" << std::dec;
    }
    
    // Test 3: Combinational property - immediate response to address change
    std::cout << "\nTest 3: Combinational logic (immediate response)\n";
    
    int test_sequence[10] = {0, 15, 1, 13, 2, 10, 3, 14, 4, 12};

    for (int i = 0; i < sizeof(test_sequence) / sizeof(test_sequence[0]); i++) {
        rom->address = test_sequence[i];
        rom->eval();
        tfp->dump(time++);
        
        expected = memory[test_sequence[i]];
        actual = rom->instruction;

        if (actual != expected) {
            std::cerr << "  Error \t Expected 0x" << std::setfill('0') << std::setw(8) << std::hex << expected 
                      << ", got 0x" << std::setfill('0') << std::setw(8) << std::hex << actual << std::dec << "\n";
            return 1;
        }
        std::cout << "  ok \t Address " << std::setfill('0') << std::setw(2) << std::dec << test_sequence[i] 
                  << ": 0x" << std::setfill('0') << std::setw(8) << std::hex << actual 
                  << "\t (expected 0x" << std::setfill('0') << std::setw(8) << std::hex << expected << ")\n" << std::dec;
    }
    
    // Test 4: All addresses are readable
    std::cout << "\nTest 4: Full address space coverage\n";
    for (int addr = 0; addr < MEMORY_SIZE; addr++) {
        rom->address = addr;
        rom->eval();
        tfp->dump(time++);
        
        expected = memory[addr];
        actual = rom->instruction;
        if (actual != expected) {
            std::cerr << "  Error \t Expected 0x" << std::setfill('0') << std::setw(8) << std::hex << expected 
                      << ", got 0x" << std::setfill('0') << std::setw(8) << std::hex << actual << std::dec << "\n";
            return 1;
        }
        std::cout << "  ok \t Address " << std::setfill('0') << std::setw(2) << std::dec << addr 
                  << ": 0x" << std::setfill('0') << std::setw(8) << std::hex << actual 
                  << "\t (expected 0x" << std::setfill('0') << std::setw(8) << std::hex << expected << ")\n" << std::dec;
    }
    std::cout << "  ok \t All " << MEMORY_SIZE << " addresses accessible\n";
    
    // Cleanup
    tfp->close();
    delete tfp;
    delete rom;
    
    std::cout << "\nAll tests passed!\n";
    std::cout << "VCD file: waveform_rom.vcd\n";
    return 0;
}
