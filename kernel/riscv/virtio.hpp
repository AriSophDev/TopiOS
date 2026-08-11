#pragma once

namespace virtio {

// MMIO base of the first virtio-mmio transport on QEMU "virt".
// UART is at 0x10000000, the first virtio device at 0x10001000.
constexpr unsigned long VIRTIO_BASE = 0x10001000;

// Probes the virtio transport and brings up a virtio-keyboard.
// Returns 0 on success, non-zero on failure.
int keyboard_init();
char keyboard_read();

} // namespace virtio