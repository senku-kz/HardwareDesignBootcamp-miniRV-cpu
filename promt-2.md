# Prompt: Create miniRV CPU

You are a computer architecture and Verilog RTL expert.

Task: design and implement a miniRV CPU in synthesizable Verilog (SystemVerilog allowed). miniRV is a subset of RISC-V RV32E.

Architecture requirements

* 32-bit PC, reset sets PC=0.
* Sequential fetch: PC = PC + 4 (unless a control-flow instruction changes it).
* 16 x 32-bit GPRs (x0–x15). x0 is hard-wired to zero: reads return 0, writes to x0 are ignored.
* 32-bit instruction width, little-endian.
* Byte-addressable data memory. Support word (32-bit) and byte (8-bit) loads/stores as specified.

Supported instructions (exactly 8)

1. add   rd, rs1, rs2        (R-type)
2. addi  rd, rs1, imm        (I-type, imm is sign-extended 12-bit)
3. lui   rd, imm             (U-type, writes imm[31:12]<<12)
4. lw    rd, imm(rs1)        (I-type, funct3=010)
5. lbu   rd, imm(rs1)        (I-type, funct3=100, zero-extend byte)
6. sw    rs2, imm(rs1)       (S-type, funct3=010)
7. sb    rs2, imm(rs1)       (S-type, funct3=000)
8. jalr  rd, imm(rs1)        (I-type, opcode=1100111, funct3=000)
   jalr semantics:
   rd = PC + 4
   PC = (rs1 + imm) & ~1

What to produce

1. Verilog RTL modules:

   * top module miniRV_cpu(clk, rst_n, imem_addr, imem_rdata, dmem_addr, dmem_wdata, dmem_wstrb, dmem_rdata)
   * regfile (16 regs) with 2 read ports, 1 write port, x0 hardwired to 0
   * decode/immgen
   * ALU
   * control logic
2. Use a simple single-cycle design (preferred) or a minimal multi-cycle design. If single-cycle, assume synchronous regs and combinational decode/ALU/mem control.
3. Instruction decode must use standard RV32I encodings (but only accept registers x0–x15). If rs/rd fields reference x16–x31, treat as illegal NOP (no write, PC+4).
4. Data memory interface:

   * dmem_addr: byte address
   * dmem_wdata: 32-bit write data
   * dmem_wstrb: 4-bit byte write strobe (for sw: 1111; for sb: one-hot based on addr[1:0])
   * dmem_rdata: 32-bit read data (aligned word read). For lbu, select correct byte from dmem_rdata using addr[1:0].
5. Instruction memory interface:

   * imem_addr: byte address (PC)
   * imem_rdata: 32-bit instruction
6. Provide a small self-checking testbench that:

   * loads a small program into an instruction ROM
   * initializes data memory with 0x12345678 and tests lw and lbu reading 0x78,0x56,0x34,0x12
   * tests sb by writing bytes to produce 0x90abcdef then reading back with lw
   * tests add/addi/lui
   * tests jalr by jumping to a function and returning (ra usage), verifying rd gets PC+4 and PC aligns with &~1.
7. Include comments and keep code compact, readable, and correct.

Deliverables format

* Output complete Verilog/SystemVerilog code for all modules and the testbench in one message (separated by file headers like // miniRV_cpu.sv, // regfile.sv, etc.).
* Explain briefly how the CPU works and how to run the testbench (iverilog/verilator style commands are fine).
