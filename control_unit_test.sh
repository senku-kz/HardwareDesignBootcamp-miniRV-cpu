#!/bin/bash
# Test Control Unit Decoder
rm -rf obj_dir/
verilator --cc control_unit.sv --exe control_unit_test.cpp --trace
make -C obj_dir -f Vcontrol_unit.mk
./obj_dir/Vcontrol_unit
# gtkwave waveform_cu.vcd
