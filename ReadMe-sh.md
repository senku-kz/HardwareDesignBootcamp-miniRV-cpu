## Program Counter
```shell
verilator --version
verilator --cc program_counter.sv
ls -l obj_dir/

rm -rf obj_dir/
verilator --cc program_counter.sv --exe program_counter_test.cpp --trace
make -C obj_dir -f Vprogram_counter.mk
./obj_dir/Vprogram_counter
gtkwave waveform_pc.vcd

or use shell script

```shell
./program_counter_test.sh
```


## Simple instruction memory
```shell
verilator --version
verilator --cc instruction_memory.sv
ls -l obj_dir/

rm -rf obj_dir/
verilator --cc instruction_memory.sv --exe instruction_memory_test.cpp --trace
make -C obj_dir -f Vinstruction_memory.mk
./obj_dir/Vinstruction_memory
gtkwave waveform.vcd
```

or use shell script

```shell
./instruction_memory_test.sh
```



## Decoder / Control Unit
```shell
verilator --version
verilator --cc control_unit.sv
ls -l obj_dir/

rm -rf obj_dir/
verilator --cc control_unit.sv --exe control_unit_test.cpp --trace
make -C obj_dir -f Vcontrol_unit.mk
./obj_dir/Vcontrol_unit
gtkwave waveform_cu.vcd
```
or use shell script

```shell
./control_unit_test.sh
```

## Register File
```shell
rm -rf obj_dir/
verilator --cc register_file.sv --exe register_file_test.cpp --trace
make -C obj_dir -f Vregister_file.mk
./obj_dir/Vregister_file
gtkwave waveform_rf.vcd
```

## ALU
```shell
rm -rf obj_dir/
verilator --cc alu.sv --exe alu_test.cpp --trace
make -C obj_dir -f Valu.mk
./obj_dir/Valu
gtkwave waveform_alu.vcd
```

## Immediate Extend:
назначение: привести immediate к ширине регистров
делает: sign-extend или zero-extend
лучшее имя: imm_ext или immediate_extend
почему нужен: без него инструкции li, branch невозможны
уровень ISA: часть datapath, управляется Control Unit

```shell
verilator --cc immediate_extend.sv --exe immediate_extend_test.cpp --trace
make -C obj_dir -f Vimmediate_extend.mk
./obj_dir/Vimmediate_extend
gtkwave waveform_imm.vcd
```



# Check Golden model
```shell
./golden_model_cpu.sh logisim-bin/task-1.hex
./golden_model_cpu.sh
./golden_model_cpu.sh logisim-bin/sum.hex
```

548 = 0x224

536 = 0x1218
