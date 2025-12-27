clear
echo "Clearing screen..."

echo "Removing obj_dir directory..."
rm -rf obj_dir/


echo "Compiling miniRV Verilog files..."
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

echo "Linking miniRV Verilog files..."
make -C obj_dir -f VminiRV.mk

echo "\n\nRunning ./obj_dir/VminiRV\n\n"
./obj_dir/VminiRV

# echo "Opening waveform_miniRV.vcd using GTKWave..."
# gtkwave waveform_miniRV.vcd

echo "\n\nSimulation complete. Waveform saved to waveform_miniRV.vcd\n\n"
echo "You can view it using GTKWave: gtkwave waveform_miniRV.vcd\n\n"