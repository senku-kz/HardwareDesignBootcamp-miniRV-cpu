#!/bin/bash
rm -rf obj_dir/
verilator --cc alu.sv --exe alu_test.cpp --trace
make -C obj_dir -f Valu.mk
./obj_dir/Valu
# gtkwave waveform_alu.vcd
