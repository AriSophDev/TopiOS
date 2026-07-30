#include "vga.hpp"

extern "C" void kernel_main() {

    vga::clear_screen();

    vga::set_color(0x0A);
    vga::put_line("hola");

    vga::set_color(0x0F);
    vga::put_line("Welcome to TopiOS");

    while (true) {
    }
}
