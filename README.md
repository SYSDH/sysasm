<h1 align="center">SYSASM</h1>

<p align="center">
  <img src="https://img.shields.io/badge/language-C-blue?cacheSeconds=300" alt="C">
  <a href="LICENSE"><img src="https://img.shields.io/github/license/sysdh/sysasm?cacheSeconds=300" alt="License"></a>
  <img src="https://img.shields.io/github/stars/sysdh/sysasm?cacheSeconds=300" alt="GitHub stars">
  <a href="https://github.com/Artxzzzz/sysasm/releases/latest">
    <img src="https://img.shields.io/github/v/release/Artxzzzz/sysasm?cacheSeconds=300" alt="Latest release">
  </a>
</p>

<br>

## 📝 Description

**SYSASM** is the official 32-bit assembler for the **SYSDH Software Suite**. Its primary job is to translate source code written in the **HASM** language into executable binary files for the **SYSVM**.

Built to replace the "peasant way" of hardcoding hexadecimal bytes, SYSASM features a custom lexer, label resolution, and an intelligent code generator that handles 32-bit memory addressing and register-specific opcodes automatically.

<br>

## 🚀 Features

*   **HASM Syntax:** Support for the official SYSDH assembly language.
*   **Label System:** Define jump targets with `label:` and reference them anywhere.
*   **Pointer support ($):** Explicit syntax for memory operations using registers as pointers.
*   **Smart Opcodes:** Automatically switches between Immediate and Register versions of instructions.
*   **32-bit Architecture:** Handles 4-byte values and addresses natively.
*   **Entry Point Control:** Use labels (`_main:`) or directives (`.entry`) to define where execution starts.

<br>

## 🛠️ HASM Instruction Set

| Mnemonic  | Args | Format               | Description                                          | Opcode (Hex) |
| :-------- | :--: | :------------------- | :--------------------------------------------------- | :----------- |
| **mov**   |   2  | `mov r, value`       | Move a value or a register into a register           | 0x10 |
| **add**   |   2  | `add r, value`       | Add a value or another register to a register        | 0x20 |
| **sub**   |   2  | `sub r, value`       | Subtract a value or another register from a register | 0x22 |
| **jz**    |   2  | `jz r, address`      | Jump to address if register equals zero              | 0x30 |
| **jnz**   |   2  | `jnz r, address`     | Jump to address if register is not zero              | 0x31 |
| **jmp**   |   1  | `jmp address`        | Unconditional jump to an address                     | 0x32 |
| **out**   |   2  | `out r, mode`        | Output register (mode: 0=number, 1=char)             | 0x40 |
| **in**    |   1  | `in r`               | Read input from stdin into a register                | 0x42 |
| **push**  |   1  | `push r`             | Push a value or register onto the stack              | 0x50 |
| **pop**   |   1  | `pop r`              | Pop value from the stack into a register             | 0x52 |
| **load**  |   2  | `load $r, r2`        | Load value from memory (pointer) into register       | 0x60 |
| **store** |   2  | `store $r, r2`       | Store register value into memory (pointer)           | 0x62 |
| **exit**  |   0  | `exit`               | Terminate the program                                | 0xFF |

*Note: Use `$` before a register to indicate it should be treated as a memory pointer.*

<br>

## 🧪 Registers

The **HASM** language uses 8 general-purpose 32-bit registers:
`h`, `he`, `li`, `be`, `b`, `c`, `n`, `o`.

<br>

## 💻 How to compile SYSASM

```bash
# Clone the repository
git clone https://github.com/Artxzzzz/sysasm.git
cd sysasm

# Compile the assembler
gcc src/**/*.c -o sysasm
```

<br>

## ▶️ How to use

To assemble a file:

```bash
./sysasm <file.hasm> -o <output.bin>

# Options
./sysasm --help      # Show CLI help
./sysasm --version   # Show SYSASM version (0.2.2-beta)
```

### 📄 HASM Example (Complex Logic)
Example of a memory-heavy operation (Brainfuck-style logic):

```asm
_main:
  ; Set memory pointers
  mov c, 10000    ; Instruction pointer
  mov h, 20000    ; Data pointer

instructionsCall:
    load $c, he  ; Load instruction at address 'c' into 'he'
    add c, 4     ; Move pointer to next 32-bit instruction

    jz he, end ; If char are terminator, exit the program
    
    ; Check for '+' (ASCII 43)
    sub he, 43
    jz he, plus
    add he, 43
    
    ; ... other logic ...
    jmp instructionsCall

plus:
    load $h, li  ; Get data at current pointer
    add li, 1    ; Increment
    store $h, li ; Save back
    jmp instructionsCall
```

<br>

## ✍️ How to contribute

1.  Fork the repository.
2.  Create a branch: `git checkout -b feature/new-logic`.
3.  Commit your changes: `git commit -m "feat: improve register mapping"`.
4.  Push to the branch: `git push origin feature/new-logic`.
5.  Open a **Pull Request**.

<br>

---
<p align="center">
  Part of the <b>SYSDH Software Suite</b><br>
  Developed by <a href="https://github.com/Artxzzzz">Artxzzzz</a>
</p>
