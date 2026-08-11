#include "uart.hpp"
#include <stdint.h>

// ns16550 register offsets (relative to UART_BASE)
#define RHR 0x00 // Receive Holding Register (read)
#define THR 0x00 // Transmit Holding Register (write)
#define IER 0x01 // Interrupt Enable Register
#define FCR 0x02 // FIFO Control Register
#define LCR 0x03 // Line Control Register
#define LSR 0x05 // Line Status Register
#define DLL 0x00 // Divisor Latch Low  (when DLAB set)
#define DLM 0x01 // Divisor Latch High (when DLAB set)

namespace {
volatile uint8_t* reg(uintptr_t off) {
    return reinterpret_cast<volatile uint8_t*>(uart::UART_BASE + off);
}
} // namespace

namespace uart {

void init() {
    // Disable interrupts
    *reg(IER) = 0x00;
    // DLAB = 1, select 8 bits, no parity, one stop bit
    *reg(LCR) = 0x80;
    // Divisor 3 => 38400 baud (matches the x86 serial driver)
    *reg(DLL) = 0x03;
    *reg(DLM) = 0x00;
    *reg(LCR) = 0x03;
    // Enable and clear FIFOs, 14-byte threshold
    *reg(FCR) = 0xC7;
}

void put_char(char c) {
    // Poll THR empty (bit 5 of the line status register)
    while ((*reg(LSR) & 0x20) == 0)
        ;
    *reg(THR) = static_cast<uint8_t>(c);
}

void put_string(const char* s) {
    while (*s) {
        put_char(*s++);
    }
}

char get_char() {
    // Poll data ready (bit 0 of the line status register)
    while ((*reg(LSR) & 0x01) == 0)
        ;
    return static_cast<char>(*reg(RHR));
}

} // namespace uart