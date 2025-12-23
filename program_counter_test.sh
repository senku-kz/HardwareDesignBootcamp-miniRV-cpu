rm -rf obj_dir/
verilator --cc program_counter.sv --exe program_counter_test.cpp --trace
make -C obj_dir -f Vprogram_counter.mk
./obj_dir/Vprogram_counter
# gtkwave waveform_pc.vcd