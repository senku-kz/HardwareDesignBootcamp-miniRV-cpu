// tb_miniRV.cpp
// C++ wrapper for Verilator simulation
//
// NOTE: Verilator has limited support for SystemVerilog testbenches with
// $display and $finish. For this testbench, iverilog is strongly recommended.
//
// If you want to use Verilator, you may need to:
// 1. Check what header Verilator generates (likely Vtb_miniRV.h or VminiRV_cpu.h)
// 2. Update the include below to match
// 3. Adjust the class name accordingly

#include "verilated.h"
#include "verilated_timing.h"

// Uncomment and adjust based on what Verilator actually generates:
// #include "Vtb_miniRV.h"  // If tb_miniRV is recognized as top module
// #include "VminiRV_cpu.h" // If miniRV_cpu is recognized as top module

vluint64_t main_time = 0;

double sc_time_stamp() {
    return main_time;
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Verilated::timingOn(true);

    // This is a placeholder - adjust based on actual Verilator output
    // For SystemVerilog testbenches, use iverilog instead:
    printf("ERROR: Verilator C++ wrapper needs adjustment based on generated headers.\n");
    printf("RECOMMENDED: Use iverilog for SystemVerilog testbenches:\n");
    printf("  iverilog -g2012 -o miniRV_tb miniRV_cpu.sv regfile.sv alu.sv decode.sv control.sv tb_miniRV.sv\n");
    printf("  vvp miniRV_tb\n");
    
    return 1;
}

