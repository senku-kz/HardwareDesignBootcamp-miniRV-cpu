rm -rf obj_dir/
verilator --cc instruction_memory.sv --exe instruction_memory_test.cpp --trace
make -C obj_dir -f Vinstruction_memory.mk
./obj_dir/Vinstruction_memory
gtkwave waveform_rom.vcd