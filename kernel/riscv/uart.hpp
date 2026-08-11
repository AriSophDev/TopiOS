#pragma once

#include <stdint.h>

namespace uart {

// QEMU "virt" machine exposes a single ns16550 UART at 0x10000000.
constexpr uintptr_t UART_BASE = 0x10000000;

void init();
void put_char(char c);
void put_string(const char* s);
char get_char();

} // namespace uart
