#!/usr/bin/env bash
# Build a bootable TopiOS ISO for x86_64 (GRUB + Multiboot2).
#
# Requirements: grub-mkrescue and xorriso. On macOS it's easier to run this
# inside the docker image (see docker/Dockerfile). On Debian/Ubuntu:
#   sudo apt install grub-pc-bin grub-common xorriso
set -euo pipefail

KERNEL="${1:-build-x86/kernel.elf}"
ISO="${2:-topios.iso}"
STAGE="${3:-iso}"

if [ ! -f "$KERNEL" ]; then
    echo "Kernel not found: $KERNEL"
    echo "Build it first with:"
    echo "  cmake -B build-x86 -DTOPIOS_ARCH=x86_64 -DCMAKE_TOOLCHAIN_FILE=scripts/toolchain-x86-64.cmake"
    echo "  cmake --build build-x86"
    exit 1
fi
if ! command -v grub-mkrescue >/dev/null 2>&1; then
    echo "grub-mkrescue not found. Install grub-pc-bin grub-common xorriso (or use the docker image)."
    exit 1
fi

mkdir -p "$STAGE/boot/grub"
cp "$KERNEL" "$STAGE/boot/kernel.elf"
grub-mkrescue -o "$ISO" "$STAGE"

echo "ISO written to: $ISO"