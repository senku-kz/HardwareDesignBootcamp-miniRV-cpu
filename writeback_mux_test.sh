#!/bin/bash
# Test Writeback Multiplexer
rm -rf obj_dir/
verilator --cc writeback_mux.sv --exe writeback_mux_test.cpp --trace
make -C obj_dir -f Vwriteback_mux.mk
./obj_dir/Vwriteback_mux
# gtkwave waveform_writeback_mux.vcd

