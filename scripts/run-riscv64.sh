#!/usr/bin/env bash
# Boot TopiOS (RISC-V build) under QEMU's "virt" machine.
#
# Usage:
#   scripts/run-riscv64.sh
#   TOPIOS_BIOS=<path-to-opensbi> scripts/run-riscv64.sh
set -euo pipefail

KERNEL="${1:-build-riscv64/kernel.elf}"
BIOS="${TOPIOS_BIOS:-/usr/local/share/qemu/opensbi-riscv64-generic-fw_dynamic.bin}"

if [ ! -f "$KERNEL" ]; then
    echo "Kernel not found: $KERNEL"
    echo "Build it first with:"
    echo "  cmake -B build-riscv64 -DTOPIOS_ARCH=riscv64 -DCMAKE_TOOLCHAIN_FILE=scripts/toolchain-riscv64.cmake"
    echo "  cmake --build build-riscv64"
    exit 1
fi

exec qemu-system-riscv64 \
    -machine virt \
    -cpu rv64 \
    -bios "$BIOS" \
    -device virtio-keyboard-device \
    -kernel "$KERNEL" \
    -nographic \
    -serial mon:stdio