# miniRV CPU Implementation

## Overview

This is a complete implementation of a miniRV CPU, which is a subset of RISC-V RV32E. The CPU supports 8 instructions and uses a single-cycle design.

## Architecture

- **32-bit PC**: Initialized to 0 on reset, increments by 4 for sequential execution
- **16 x 32-bit GPRs (x0-x15)**: x0 is hardwired to zero
- **32-bit instruction width**: Little-endian byte order
- **Byte-addressable memory**: Supports word (32-bit) and byte (8-bit) loads/stores

## Supported Instructions

1. **add** rd, rs1, rs2 - Add two registers
2. **addi** rd, rs1, imm - Add immediate (12-bit sign-extended)
3. **lui** rd, imm - Load upper immediate (imm[31:12] << 12)
4. **lw** rd, imm(rs1) - Load word
5. **lbu** rd, imm(rs1) - Load byte unsigned (zero-extended)
6. **sw** rs2, imm(rs1) - Store word
7. **sb** rs2, imm(rs1) - Store byte
8. **jalr** rd, imm(rs1) - Jump and link register (PC = (rs1 + imm) & ~1, rd = PC + 4)

## Module Structure

- **miniRV_cpu.sv**: Top-level CPU module
- **regfile.sv**: 16-register file with 2 read ports, 1 write port
- **alu.sv**: Arithmetic logic unit (addition)
- **decode.sv**: Instruction decoder and immediate generator
- **control.sv**: Control logic for instruction execution
- **tb_miniRV.sv**: Comprehensive testbench

## How the CPU Works

The CPU uses a single-cycle design:

1. **Fetch**: Instruction is read from instruction memory at address PC
2. **Decode**: Instruction is decoded to extract opcode, register fields, and immediate values
3. **Register Read**: Source registers (rs1, rs2) are read from the register file
4. **Execute**: 
   - ALU performs addition (rs1 + rs2 or rs1 + immediate)
   - For memory operations, address is computed as rs1 + immediate
   - For lui, immediate is shifted left by 12 bits
5. **Memory Access**: 
   - Loads: Read data from memory, select byte for lbu
   - Stores: Write data to memory with appropriate byte strobes
6. **Writeback**: Result is written back to destination register (rd)
7. **PC Update**: 
   - Sequential: PC = PC + 4
   - jalr: PC = (rs1 + imm) & ~1

The control unit generates signals based on the instruction opcode and funct3 field to control:
- Register write enable
- Memory read/write enables
- ALU source selection (register vs immediate)
- Writeback source selection (ALU, memory, PC+4, or immediate)
- PC source selection (sequential vs jalr)

## Running the Testbench

### Using iverilog (Icarus Verilog) - Recommended

```bash
iverilog -g2012 -o miniRV_tb miniRV_cpu.sv regfile.sv alu.sv decode.sv control.sv tb_miniRV.sv
vvp miniRV_tb
```

This is the simplest option as iverilog natively supports SystemVerilog testbenches with `$display` and `$finish`.

### Using Verilator (Advanced - Not Recommended for This Testbench)

Verilator has limited support for SystemVerilog testbenches with `$display` and `$finish`. The testbench is designed for iverilog. If you must use Verilator:

1. You'll need to rewrite the testbench in C++ or use Verilator's limited SystemVerilog support
2. The C++ wrapper (`tb_miniRV.cpp`) is provided as a starting point but may need adjustment
3. Check Verilator's generated headers in `obj_dir/` to determine the correct class names

```bash
# This may not work directly due to SystemVerilog testbench limitations
verilator --cc --exe --build -j 0 -Wall --timing miniRV_cpu.sv regfile.sv alu.sv decode.sv control.sv tb_miniRV.sv tb_miniRV.cpp
./obj_dir/Vtb_miniRV 
```

### Using ModelSim/QuestaSim

```bash
vlog -sv miniRV_cpu.sv regfile.sv alu.sv decode.sv control.sv tb_miniRV.sv
vsim -c tb_miniRV -do "run -all; quit"
```

## Test Cases

The testbench validates:

1. **Memory Load Tests**:
   - `lw`: Loads word 0x12345678 from memory
   - `lbu`: Loads individual bytes (0x78, 0x56, 0x34, 0x12) and verifies zero-extension

2. **Memory Store Tests**:
   - `sb`: Stores individual bytes to create 0x90ABCDEF
   - `lw`: Verifies the word can be read back correctly

3. **Arithmetic Tests**:
   - `addi`: Tests immediate addition (5, 3)
   - `add`: Tests register addition (5 + 3 = 8)
   - `lui`: Tests load upper immediate (0x12345000)

4. **Control Flow Tests**:
   - `jalr`: Tests jump and link register
     - Verifies rd gets PC+4
     - Verifies PC alignment (LSB cleared)
     - Tests function call and return pattern

## Notes

- Illegal instructions (unsupported opcodes or registers x16-x31) are treated as NOPs
- x0 is always zero: reads return 0, writes are ignored
- All memory addresses are byte-addressable
- Word accesses must be aligned (address[1:0] = 0 for optimal performance, though the design handles unaligned)



# Task 2


add rd, rs1, rs2
r[x2]=r[x1]+r[x10]
0000000_01010_00001_000_00010_0110011