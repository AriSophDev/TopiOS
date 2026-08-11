#include "idt.hpp"
#include "pic.hpp"
#include "interrupt.hpp"
#include <stdint.h>

namespace {

struct __attribute__((packed)) IdtEntry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  ist;
    uint8_t  type_attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;
};

struct __attribute__((packed)) Idtr {
    uint16_t limit;
    uint64_t base;
};

constexpr int IDT_ENTRIES = 256;
alignas(16) IdtEntry idt_table[IDT_ENTRIES];
Idtr idtr;

extern "C" const uint64_t isr_stub_table[48];

void set_entry(int vector, uint64_t handler, uint8_t type_attr) {
    idt_table[vector].offset_low  = handler & 0xFFFF;
    idt_table[vector].selector    = 0x08;
    idt_table[vector].ist         = 0;
    idt_table[vector].type_attr   = type_attr;
    idt_table[vector].offset_mid  = (handler >> 16) & 0xFFFF;
    idt_table[vector].offset_high = (handler >> 32) & 0xFFFFFFFF;
    idt_table[vector].zero        = 0;
}

void default_handler(InterruptFrame* frame) {
    (void)frame;
}

} // namespace

namespace idt {

void init() {
    // Clear IDT
    for (int i = 0; i < IDT_ENTRIES; i++) {
        set_entry(i, 0, 0);
    }

    // Set up exception handlers (vectors 0-31)
    for (int i = 0; i < 32; i++) {
        set_entry(i, isr_stub_table[i], 0x8E); // Interrupt Gate
    }

    // Set up hardware IRQ handlers (vectors 32-47)
    for (int i = 32; i < 48; i++) {
        set_entry(i, isr_stub_table[i], 0x8E); // Interrupt Gate
    }

    // Register default handlers for all exceptions
    for (int i = 0; i < 32; i++) {
        interrupt::register_handler(i, default_handler);
    }

    // Remap PIC: IRQ 0-15 → vectors 0x20-0x2F
    pic::remap(0x20, 0x28);

    // Mask all IRQs initially
    pic::mask_all();

    // Load IDT
    idtr.limit = sizeof(IdtEntry) * IDT_ENTRIES - 1;
    idtr.base  = reinterpret_cast<uint64_t>(&idt_table[0]);
    asm volatile("lidt %0" : : "m"(idtr));
}

} // namespace idt
