## Boot Process & Compilation Pipeline

This section describes how TopiOS transforms from raw assembly and C++ source code into an executable kernel loaded by GRUB.

### Build & Execution Flow

1. **`boot.asm` `boot.o`**
   - The assembly bootstrap file is compiled into an object file.
   - Sets up the Multiboot header and sets the CPU entry point (`_start`).

2. **`kernel.cpp`  `kernel.o`**
   - The main C++ kernel code is compiled with a bare-metal target setup (freestanding mode, no standard library).

3. **Linker Script  `kernel.elf`**
   - The GNU Linker (`ld`) merges `boot.o` and `kernel.o` into a single ELF binary using a custom linker script.
   - Resolves symbol references and ensures memory sections (`.text`, `.rodata`, `.data`, `.bss`) are laid out at the correct virtual addresses.

4. **GRUB Loading**
   - The GRUB bootloader reads `kernel.elf`

5. **CPU Execution**
   - Control jumps to the entry point `_start` inside `boot.o`.
   - The assembly code sets up the stack pointer (`esp`) and transfers execution to the C++ kernel main function (`kernel_main`).
