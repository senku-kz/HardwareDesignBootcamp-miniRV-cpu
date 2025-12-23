#!/bin/bash
# Test Control Unit Decoder
rm -rf obj_dir/
verilator --cc control_unit.sv --exe control_unit_test.cpp --trace
make -C obj_dir -f Vcontrol_unit.mk
./obj_dir/Vcontrol_unit
# gtkwave waveform_cu.vcd

# # Test 1: R-Type (Register-Register) (OP)
# rm -rf obj_dir/
# verilator --cc control_unit.sv --exe control_unit_r_type_test.cpp --trace
# make -C obj_dir -f Vcontrol_unit.mk
# ./obj_dir/Vcontrol_unit
# # gtkwave waveform_cu.vcd

# # Test 2: I-Type (Immediate/Loads/JALR) (JALR)
# rm -rf obj_dir/
# verilator --cc control_unit.sv --exe control_unit_i_type_test.cpp --trace
# make -C obj_dir -f Vcontrol_unit.mk
# ./obj_dir/Vcontrol_unit
# # gtkwave waveform_cu.vcd