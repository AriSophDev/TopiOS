#pragma once
#include <stdint.h>

struct InterruptFrame {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
    uint64_t vector;
    uint64_t error_code;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
};

using InterruptHandler = void (*)(InterruptFrame*);

namespace interrupt {
void init();
void register_handler(uint8_t vector, InterruptHandler handler);
} // namespace interrupt
