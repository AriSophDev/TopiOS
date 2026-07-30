#pragma once
#include <stddef.h>
#include <stdint.h>

namespace vga {
    void put_char(char c);
    void clear_screen();
    void put_string(const char* str);
    void put_line(const char* str);
    void put_color();
    void set_color(uint8_t color);
}