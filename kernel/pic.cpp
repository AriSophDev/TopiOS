#include "pic.hpp"
#include <stdint.h>

namespace {
constexpr uint16_t PIC1_CMD  = 0x20;
constexpr uint16_t PIC1_DATA = 0x21;
constexpr uint16_t PIC2_CMD  = 0xA0;
constexpr uint16_t PIC2_DATA = 0xA1;

inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

inline void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" :: "a"(val), "Nd"(port));
}

inline void io_wait() {
    outb(0x80, 0);
}
} // namespace

namespace pic {

void remap(uint8_t master_offset, uint8_t slave_offset) {
    uint8_t mask1 = inb(PIC1_DATA);
    uint8_t mask2 = inb(PIC2_DATA);

    // ICW1: begin initialization
    outb(PIC1_CMD, 0x11);   io_wait();
    outb(PIC2_CMD, 0x11);   io_wait();

    // ICW2: vector offsets
    outb(PIC1_DATA, master_offset); io_wait();
    outb(PIC2_DATA, slave_offset);  io_wait();

    // ICW3: cascade wiring
    outb(PIC1_DATA, 1 << 2); io_wait();
    outb(PIC2_DATA, 0x02);   io_wait();

    // ICW4: 8086 mode
    outb(PIC1_DATA, 0x01); io_wait();
    outb(PIC2_DATA, 0x01); io_wait();

    // Restore masks
    outb(PIC1_DATA, mask1);
    outb(PIC2_DATA, mask2);
}

void send_eoi(uint8_t irq) {
    if (irq >= 8) {
        outb(PIC2_CMD, 0x20);
    }
    outb(PIC1_CMD, 0x20);
}

void mask(uint8_t irq) {
    uint16_t port = (irq < 8) ? PIC1_DATA : PIC2_DATA;
    uint8_t bit = irq % 8;
    uint8_t mask_val = inb(port) | (1 << bit);
    outb(port, mask_val);
}

void unmask(uint8_t irq) {
    uint16_t port = (irq < 8) ? PIC1_DATA : PIC2_DATA;
    uint8_t bit = irq % 8;
    uint8_t mask_val = inb(port) & ~(1 << bit);
    outb(port, mask_val);
}

void mask_all() {
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}

} // namespace pic
