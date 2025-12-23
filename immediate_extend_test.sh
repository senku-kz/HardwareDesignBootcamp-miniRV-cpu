#!/bin/bash
rm -rf obj_dir/
verilator --cc immediate_extend.sv --exe immediate_extend_test.cpp --trace
make -C obj_dir -f Vimmediate_extend.mk
./obj_dir/Vimmediate_extend
# gtkwave waveform_imm.vcd
