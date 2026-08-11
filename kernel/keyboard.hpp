#pragma once
#include <stdint.h>

struct InterruptFrame;

namespace keyboard {
void init();
char read();
int available();
void irq_handler(InterruptFrame* frame);
} // namespace keyboard
