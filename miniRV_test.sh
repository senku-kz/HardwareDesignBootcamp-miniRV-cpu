#!/bin/bash
# Test miniRV CPU
rm -rf obj_dir/
verilator --cc miniRV.sv instruction_fetch.sv control_unit.sv immediate_generator.sv register_file.sv alu.sv writeback_mux.sv --exe miniRV_test.cpp --trace
make -C obj_dir -f VminiRV.mk
./obj_dir/VminiRV
# gtkwave waveform_miniRV.vcd

