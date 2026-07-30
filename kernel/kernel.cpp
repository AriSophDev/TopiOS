#include <stdint.h>

extern "C" void kernel_main() {
    volatile uint16_t* vga = (volatile uint16_t*)0xB8000;

    vga[0] = 0x0F48; // H
    vga[1] = 0x0F69; // i

    while (true) {
        asm volatile("hlt");
    }
}
