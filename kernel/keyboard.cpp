#include "keyboard.hpp"
#include <stdint.h>

namespace {
constexpr uint16_t KB_DATA   = 0x60;
constexpr uint16_t KB_STATUS = 0x64;

inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

inline void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" :: "a"(val), "Nd"(port));
}

static char scancode_to_char(uint8_t sc, bool shift) {
    switch (sc) {
        case 0x02: return shift ? '!' : '1';
        case 0x03: return shift ? '@' : '2';
        case 0x04: return shift ? '#' : '3';
        case 0x05: return shift ? '$' : '4';
        case 0x06: return shift ? '%' : '5';
        case 0x07: return shift ? '^' : '6';
        case 0x08: return shift ? '&' : '7';
        case 0x09: return shift ? '*' : '8';
        case 0x0A: return shift ? '(' : '9';
        case 0x0B: return shift ? ')' : '0';
        case 0x0C: return shift ? '_' : '-';
        case 0x0D: return shift ? '+' : '=';
        case 0x0E: return '\b';
        case 0x0F: return '\t';
        case 0x10: return shift ? 'Q' : 'q';
        case 0x11: return shift ? 'W' : 'w';
        case 0x12: return shift ? 'E' : 'e';
        case 0x13: return shift ? 'R' : 'r';
        case 0x14: return shift ? 'T' : 't';
        case 0x15: return shift ? 'Y' : 'y';
        case 0x16: return shift ? 'U' : 'u';
        case 0x17: return shift ? 'I' : 'i';
        case 0x18: return shift ? 'O' : 'o';
        case 0x19: return shift ? 'P' : 'p';
        case 0x1A: return shift ? '{' : '[';
        case 0x1B: return shift ? '}' : ']';
        case 0x1C: return '\n';
        case 0x1E: return shift ? 'A' : 'a';
        case 0x1F: return shift ? 'S' : 's';
        case 0x20: return shift ? 'D' : 'd';
        case 0x21: return shift ? 'F' : 'f';
        case 0x22: return shift ? 'G' : 'g';
        case 0x23: return shift ? 'H' : 'h';
        case 0x24: return shift ? 'J' : 'j';
        case 0x25: return shift ? 'K' : 'k';
        case 0x26: return shift ? 'L' : 'l';
        case 0x27: return shift ? ':' : ';';
        case 0x28: return shift ? '"' : '\'';
        case 0x29: return shift ? '~' : '`';
        case 0x2B: return shift ? '|' : '\\';
        case 0x2C: return shift ? 'Z' : 'z';
        case 0x2D: return shift ? 'X' : 'x';
        case 0x2E: return shift ? 'C' : 'c';
        case 0x2F: return shift ? 'V' : 'v';
        case 0x30: return shift ? 'B' : 'b';
        case 0x31: return shift ? 'N' : 'n';
        case 0x32: return shift ? 'M' : 'm';
        case 0x33: return shift ? '<' : ',';
        case 0x34: return shift ? '>' : '.';
        case 0x35: return shift ? '?' : '/';
        case 0x39: return ' ';
        default:   return 0;
    }
}

static bool key_pressed[128] = {};
static bool shift = false;
} // namespace

namespace keyboard {

void init() {
    outb(0x64, 0xAE);
}

int available() {
    return (inb(KB_STATUS) & 1) != 0;
}

char poll() {
    if (!available()) return 0;
    uint8_t sc = inb(KB_DATA);
    if (sc == 0xE0) return 0;
    bool is_make = (sc & 0x80) == 0;
    uint8_t code = sc & 0x7F;
    if (code >= 128) return 0;

    if (code == 0x2A) { shift = is_make; return 0; }
    if (code == 0x36) { shift = is_make; return 0; }

    if (!is_make) return 0;

    return scancode_to_char(code, shift);
}

char read() {
    char c;
    while ((c = poll()) == 0) {
        asm volatile("pause");
    }
    return c;
}

} // namespace keyboard