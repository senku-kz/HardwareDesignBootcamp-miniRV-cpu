rm -rf obj_dir/
verilator --cc register_file.sv --exe register_file_test.cpp --trace
make -C obj_dir -f Vregister_file.mk
./obj_dir/Vregister_file
gtkwave waveform_rf.vcd