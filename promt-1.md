# Prompt: *Create miniRV CPU*

You are a computer architecture and RISC-V expert.

Your task is to **design and implement a miniRV CPU**, which is a subset of **RISC-V RV32E**, following the specification below.

**Architecture requirements**

* ISA: **miniRV (subset of RISC-V RV32E)**
* PC width: **32 bits**
* PC initialization: **PC = 0**
* PC update: **PC = PC + 4** for sequential execution
* Register file:

  * **16 general-purpose registers (x0–x15)**
  * Each register is **32 bits**
  * **x0 is hard-wired to zero**
* Instruction width: **32 bits**
* Endianness: **little-endian**
* Memory model:

  * Byte-addressable memory
  * Word accesses are 32 bits

**Supported instructions (exactly 8)**

1. `add   rd, rs1, rs2`
2. `addi  rd, rs1, imm`
3. `lui   rd, imm`
4. `lw    rd, imm(rs1)`
5. `lbu   rd, imm(rs1)`
6. `sw    rs2, imm(rs1)`
7. `sb    rs2, imm(rs1)`
8. `jalr  rd, imm(rs1)`

All instructions must strictly follow **RV32E/RV32I encoding formats** (R, I, S, U types).

**Instruction semantics**

* `add` : `R[rd] = R[rs1] + R[rs2]`
* `addi`: `R[rd] = R[rs1] + signext(imm)`
* `lui` : `R[rd] = imm << 12`
* `lw`  : `R[rd] = M[R[rs1] + imm][31:0]`
* `lbu` : `R[rd] = zeroext(M[R[rs1] + imm][7:0])`
* `sw`  : `M[R[rs1] + imm] = R[rs2][31:0]`
* `sb`  : `M[R[rs1] + imm][7:0] = R[rs2][7:0]`
* `jalr`:

  ```
  R[rd] = PC + 4
  PC = (R[rs1] + imm) & ~1
  ```

**What you must produce**

1. A **clear block-level CPU architecture** (PC, IF, decoder, regfile, ALU, memory)
2. Instruction decode logic (opcode, funct3, funct7 handling)
3. Control-signal table per instruction
4. One of the following implementations (choose one, or more if requested):

   * Logisim-evolution schematic
   * Verilog RTL (synthesizable)
   * C/C++ functional simulator
5. Simple **test programs** validating:

   * arithmetic (`add`, `addi`)
   * control flow (`jalr`)
   * memory (`lw`, `lbu`, `sw`, `sb`)
6. Ensure **x0 always remains zero**, regardless of writes

**Constraints**

* No pipelining (single-cycle or multi-cycle is fine)
* No exceptions or CSR support
* No unsupported RISC-V instructions

Explain your design decisions clearly and keep the implementation minimal but correct.
