#include "uart.hpp"
#include "virtio.hpp"

extern "C" void kernel_main() {
    uart::init();
    uart::put_string("TopiOS RISC-V booted!\n");
    uart::put_string("Hola\n");

    virtio::keyboard_init();

    while (true) {
        char c = virtio::keyboard_read();
        if (c != 0) {
            uart::put_char(c);
        }
    }
}