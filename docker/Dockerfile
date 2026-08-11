FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    nasm \
    grub-pc-bin \
    grub-common \
    xorriso \
    qemu-system-x86 \
    qemu-system-misc \
    && rm -rf /var/lib/apt/lists/*

# RISC-V bare-metal cross toolchain (provides riscv64-unknown-elf-gcc)
RUN apt-get update && apt-get install -y \
    gcc-riscv64-unknown-elf \
    g++-riscv64-unknown-elf \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace