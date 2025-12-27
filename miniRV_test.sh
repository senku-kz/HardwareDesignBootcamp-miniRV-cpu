rm -rf obj_dir/

verilator --cc \
  miniRV.sv \
  alu.sv \
  control_unit.sv \
  immediate_generator.sv \
  instruction_fetch.sv \
  program_counter.sv \
  register_file.sv \
  writeback_mux.sv \
  --exe miniRV_test.cpp golden_model_cpu.cpp \
  --top-module miniRV \
  --trace

# Now link
make -C obj_dir -f VminiRV.mk

echo "\n\nRunning miniRV testbench...\n\n"
./obj_dir/VminiRV

# gtkwave waveform_miniRV.vcd

echo "\n\nSimulation complete. Waveform saved to waveform_miniRV.vcd\n\n"
echo "You can view it using GTKWave: gtkwave waveform_miniRV.vcd\n\n"