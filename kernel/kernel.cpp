#include "vga.hpp"
#include "serial.hpp"
#include "keyboard.hpp"

using vga::put_line;

extern "C" void kernel_main() {
  serial::init();
  serial::put_string("TopiOS booted!\n");

  vga::clear_screen();
  vga::set_color(0x0A);
  put_line("Hola");
  vga::set_color(0x0F);

  keyboard::init();
  vga::put_line("TopiOS - Keyboard driver ready\n");

  while (true) {
      char c = keyboard::read();
      if (c == '\r') c = '\n';
      vga::put_char(c);
      serial::put_char(c);
  }
}