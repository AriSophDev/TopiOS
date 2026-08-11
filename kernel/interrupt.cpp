#include "interrupt.hpp"
#include "pic.hpp"
#include "serial.hpp"
#include <stdint.h>

namespace {
InterruptHandler handlers[256] = {};
} // namespace

extern "C" void interrupt_dispatcher(InterruptFrame* frame) {
    uint8_t vector = static_cast<uint8_t>(frame->vector);

    if (handlers[vector]) {
        handlers[vector](frame);
    } else {
        serial::put_string("[ISR] Unhandled vector: ");
        serial::put_char('0' + vector / 10);
        serial::put_char('0' + vector % 10);
        serial::put_char('\n');
    }

    // Send EOI for hardware IRQs (vectors 32-47)
    if (vector >= 32 && vector < 48) {
        pic::send_eoi(vector - 32);
    }
}

namespace interrupt {

void register_handler(uint8_t vector, InterruptHandler handler) {
    handlers[vector] = handler;
}

} // namespace interrupt
