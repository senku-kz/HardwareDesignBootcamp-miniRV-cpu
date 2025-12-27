#include "golden_model_cpu.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>
#include <bitset>
#include <stdexcept>


int main(int argc, char** argv) {
    std::string hex_file = "logisim-bin/sum.hex";
    if (argc > 1) {
        hex_file = argv[1];
    }
    
    GoldenModelCPU cpu;
    
    // Load instructions into instruction memory
    std::cout << "Loading instructions from " << hex_file << " into instruction memory...\n";
    if (!cpu.loadHexFile(hex_file)) {
        return 1;
    }
    for (int i = 0; i < 10; i++) {
        std::cout << "imem[" << i << "] = 0x" << std::hex << std::setfill('0') << std::setw(8) 
                  << cpu.imem[i] << std::dec << std::endl;
    }
    
    // Also load data into data memory (as per requirement: hex file contains both instructions and data)
    std::cout << "Loading data from " << hex_file << " into data memory...\n";
    // if (!cpu.loadHexFileToDmem(hex_file)) {
    //     return 1;
    // }
    for (int i = 0; i < 10; i++) {
        std::cout << "dmem[" << i << "] = 0x" << std::hex << std::setfill('0') << std::setw(8) 
                  << cpu.dmem[i] << std::dec << std::endl;
    }
    
    // Reset CPU
    cpu.resetCPU();
    
    // Print initial state
    std::cout << "\nInitial state:\n";
    cpu.printState();
    
    // Run for a few cycles (or until program ends)
    // std::cout << "\nExecuting instructions...\n";
    // cpu.runCycles(100);  // Execute 100 instructions
    std::cout << "--------------------------------\n";
    std::cout << "Executing instructions...\n";
    for (int i = 0; i < CYCLE_LIMIT; i++) {
        std::cout << "Cycle " << i+1 << "\n";
        cpu.clockCycle();
        cpu.printState();
        std::cout << "\n";
    }
    std::cout << "--------------------------------\n";
    
    // Print final state
    std::cout << "\nFinal state:\n";
    cpu.printState();
    
    return 0;
}
