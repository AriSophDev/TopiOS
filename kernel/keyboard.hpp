#pragma once
#include <stdint.h>

namespace keyboard {
void init();
char read();
int available();
char poll();
} // namespace keyboard