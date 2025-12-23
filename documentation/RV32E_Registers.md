# Registers of RV32E

RV32E (Embedded) is a reduced version of RISC-V with only 16 general-purpose registers (x0-x15) instead of the standard 32 registers.

## Register Table

| Register Name | Alias | Role |
|---------------|-------|------|
| x0            | zero  | Hardwired to zero. Always reads as 0. Writes are ignored. |
| x1            | ra    | Return address. Used to store the return address for function calls (JALR). |
| x2            | sp    | Stack pointer. Points to the top of the stack. |
| x3            | gp    | Global pointer. Points to the global data area. |
| x4            | tp    | Thread pointer. Points to thread-local storage. |
| x5            | t0    | Temporary register 0. Caller-saved, used for temporary values. |
| x6            | t1    | Temporary register 1. Caller-saved, used for temporary values. |
| x7            | t2    | Temporary register 2. Caller-saved, used for temporary values. |
| x8            | s0/fp | Saved register 0 / Frame pointer. Callee-saved, can be used as frame pointer. |
| x9            | s1    | Saved register 1. Callee-saved, preserved across function calls. |
| x10           | a0    | Function argument 0 / Return value 0. Used to pass first argument or return value. |
| x11           | a1    | Function argument 1 / Return value 1. Used to pass second argument or return value. |
| x12           | a2    | Function argument 2. Used to pass third argument. |
| x13           | a3    | Function argument 3. Used to pass fourth argument. |
| x14           | a4    | Function argument 4. Used to pass fifth argument. |
| x15           | a5    | Function argument 5. Used to pass sixth argument. |

## Register Categories

### Zero Register (x0)
- **Always zero**: Reading x0 always returns 0
- **Writes ignored**: Writing to x0 has no effect
- **Common uses**: 
  - Discarding results: `add x0, x1, x2` (compute but don't store)
  - Loading zero: `addi x0, x0, 0` (NOP)
  - Comparisons: `add x0, x1, x2` to set flags (if implemented)

### Return Address (x1 / ra)
- **Primary use**: Stores return address from function calls
- **JALR usage**: `jalr x1, 0(x2)` saves PC+4 to x1
- **Call convention**: Should be preserved by callee if used

### Stack Pointer (x2 / sp)
- **Purpose**: Points to the top of the stack
- **Stack growth**: Typically grows downward (decrements on push)
- **Alignment**: Should be aligned to 16 bytes (4 words) for RV32E

### Global Pointer (x3 / gp)
- **Purpose**: Points to the global data section
- **Optimization**: Allows efficient access to global variables within ±2KB range

### Thread Pointer (x4 / tp)
- **Purpose**: Points to thread-local storage
- **Use case**: Multi-threaded applications

### Temporary Registers (x5-x7 / t0-t2)
- **Caller-saved**: Caller must save these if needed across function calls
- **Volatile**: Can be modified by called functions
- **Use**: Temporary calculations, intermediate values

### Saved Registers (x8-x9 / s0-s1)
- **Callee-saved**: Callee must preserve these values
- **s0/fp**: Can be used as frame pointer for stack frame management
- **Use**: Values that must persist across function calls

### Argument/Return Registers (x10-x15 / a0-a5)
- **Function arguments**: a0-a5 used to pass up to 6 arguments
- **Return values**: a0-a1 used to return values from functions
- **Caller-saved**: Can be modified by called functions
- **Convention**: 
  - a0: First argument or return value
  - a1: Second argument or return value
  - a2-a5: Additional arguments

## Register Usage Conventions

### Calling Convention Summary

**Caller-saved (temporary) registers:**
- x5-x7 (t0-t2): Temporary values
- x10-x15 (a0-a5): Arguments and return values

**Callee-saved (preserved) registers:**
- x1 (ra): Return address (if used)
- x2 (sp): Stack pointer
- x8-x9 (s0-s1): Saved registers

**Special registers:**
- x0 (zero): Always zero
- x3 (gp): Global pointer
- x4 (tp): Thread pointer

## Notes

1. **RV32E limitation**: Only registers x0-x15 exist. Attempting to use x16-x31 will result in illegal instruction or undefined behavior.

2. **Register width**: All registers are 32 bits wide.

3. **x0 behavior**: 
   - Reading x0 always returns 0
   - Writing to x0 is a no-op (instruction executes but has no effect)

4. **Stack alignment**: The stack pointer (sp) should be aligned to 16 bytes for proper function calls and system calls.

5. **ABI compliance**: These conventions follow the RISC-V Application Binary Interface (ABI) adapted for RV32E.

