#pragma once
#include <stdint.h>

namespace pic {
void remap(uint8_t master_offset, uint8_t slave_offset);
void send_eoi(uint8_t irq);
void mask(uint8_t irq);
void unmask(uint8_t irq);
void mask_all();
} // namespace pic
