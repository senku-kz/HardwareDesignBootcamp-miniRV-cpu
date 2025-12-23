rm -rf obj_dir/
verilator --cc instruction_fetch.sv --exe instruction_fetch_test.cpp --trace
make -C obj_dir -f Vinstruction_fetch.mk
./obj_dir/Vinstruction_fetch
# gtkwave waveform_instruction_fetch.vcd