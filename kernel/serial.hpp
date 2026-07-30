#pragma once
#include <stdint.h>

namespace serial {
void init();
void put_char(char c);
void put_string(const char *s);

} // namespace serial


