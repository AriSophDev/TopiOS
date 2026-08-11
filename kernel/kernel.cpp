#include "vga.hpp"
#include "serial.hpp"
#include "keyboard.hpp"
#include "idt.hpp"
#include "pic.hpp"
#include "interrupt.hpp"

using vga::put_line;

extern "C" void kernel_main() {
    serial::init();
    serial::put_string("TopiOS booted!\n");

    vga::clear_screen();
    vga::set_color(0x0A);
    put_line("TopiOS - Initializing interrupts...");
    vga::set_color(0x0F);

    idt::init();
    serial::put_string("IDT loaded\n");

    keyboard::init();
    interrupt::register_handler(33, keyboard::irq_handler);
    pic::unmask(1); // Unmask IRQ1 (keyboard)
    serial::put_string("Keyboard IRQ registered\n");

    vga::clear_screen();
    vga::set_color(0x0A);
    put_line("TopiOS - Ready");
    vga::set_color(0x0F);
    put_line("Type something:");
    put_line("");

    asm volatile("sti");

    while (true) {
        char c = keyboard::read();
        if (c == '\b') {
            vga::put_char('\b');
            vga::put_char(' ');
            vga::put_char('\b');
        } else {
            vga::put_char(c);
        }
        serial::put_char(c);
    }
}
