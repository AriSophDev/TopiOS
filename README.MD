# TopiOS

A hobby operating system written in C++.

## Targets

TopiOS currently supports two target architectures, selected with the
`TOPIOS_ARCH` CMake variable (`x86_64` by default, or `riscv64`).

### Build & run (RISC-V)

```sh
# install the cross toolchain (macOS/Homebrew)
brew install riscv64-elf-gcc riscv64-elf-binutils

# configure + build
cmake -B build-riscv64 -DTOPIOS_ARCH=riscv64 -DCMAKE_TOOLCHAIN_FILE=scripts/toolchain-riscv64.cmake
cmake --build build-riscv64

# run under QEMU "virt" (OpenSBI + virtio-keyboard)
scripts/run-riscv64.sh
```

The RISC-V kernel prints `Hola` over UART and echoes keys typed on the
`virtio-keyboard` device.

See [`docs/PORT_RISCV.md`](docs/PORT_RISCV.md) for a step-by-step walkthrough.

## Goals

- Learn operating systems
- Learn low-level programming
- Learn computer architecture
- Play some music in
- Have fun :)

## Notes

The x86_64 build currently fails at `HEAD` (pre-existing: `boot.asm` references
`stack_top`, `pml4_table`, etc. that are no longer defined). It was already
broken before the RISC-V port.