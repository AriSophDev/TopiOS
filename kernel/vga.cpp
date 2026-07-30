#include "vga.hpp"
#include <cstddef>
#include <stdint.h>

namespace {
constexpr uintptr_t VGA_ADDRESS = 0xB8000;
constexpr size_t VGA_WIDTH = 80;
constexpr size_t VGA_HEIGHT = 25;
constexpr uint8_t DEFAULT_COLOR = 0x07;

uint8_t current_color = DEFAULT_COLOR;
size_t cursor_x = 0;
size_t cursor_y = 0;
} // namespace

namespace vga {

void set_color(uint8_t color) { current_color = color; }

void put_char(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
        if (cursor_y >= VGA_HEIGHT)
            cursor_y = 0;
        return;
    }

    volatile uint8_t *video = reinterpret_cast<volatile uint8_t *>(VGA_ADDRESS);

    size_t offset = (cursor_y * VGA_WIDTH + cursor_x) * 2;
    video[offset] = static_cast<uint8_t>(c);
    video[offset + 1] = current_color;

    cursor_x++;
    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
        if (cursor_y >= VGA_HEIGHT)
            cursor_y = 0;
    }
}

void clear_screen() {
    volatile uint8_t *video = reinterpret_cast<volatile uint8_t *>(VGA_ADDRESS);
    for (size_t i = 0; i < VGA_HEIGHT * VGA_WIDTH; ++i) {
        video[i * 2] = ' ';
        video[i * 2 + 1] = current_color;
    }
    cursor_x = 0;
    cursor_y = 0;
}

void put_string(const char *str) {
    while (*str) {
        put_char(*str++);
    }
}

void put_line(const char *str) {
    put_string(str);
    put_char('\n');
}

void put_color() { set_color(DEFAULT_COLOR); }

} // namespace vga
