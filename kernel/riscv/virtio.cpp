#include "virtio.hpp"
#include "uart.hpp"
#include <stdint.h>

namespace {

constexpr uint32_t VIRT_MAGIC = 0x74726976; // "virt"
constexpr uint32_t VIRT_ID_INPUT = 18;      // virtio-input (QEMU virtio-keyboard)
constexpr uint16_t VIRT_VENDOR = 0x1AF4;

// Register offsets (linux/virtio_mmio.h)
constexpr uint32_t R_MAGIC           = 0x000;
constexpr uint32_t R_VERSION         = 0x004;
constexpr uint32_t R_DEVICE_ID       = 0x008;
constexpr uint32_t R_VENDOR_ID       = 0x00c;
constexpr uint32_t R_DEV_FEATURES    = 0x010;
constexpr uint32_t R_DEV_FEATURES_SEL= 0x014;
constexpr uint32_t R_DRV_FEATURES    = 0x020;
constexpr uint32_t R_DRV_FEATURES_SEL= 0x024;
// Legacy-only registers
constexpr uint32_t R_GUEST_PAGE_SIZE = 0x028;
constexpr uint32_t R_QUEUE_SEL       = 0x030;
constexpr uint32_t R_QUEUE_NUM_MAX   = 0x034;
constexpr uint32_t R_QUEUE_NUM       = 0x038;
constexpr uint32_t R_QUEUE_ALIGN     = 0x03c;
constexpr uint32_t R_QUEUE_PFN       = 0x040;
constexpr uint32_t R_QUEUE_NOTIFY    = 0x050;
constexpr uint32_t R_INT_STATUS      = 0x060;
constexpr uint32_t R_INT_ACK         = 0x064;
constexpr uint32_t R_STATUS          = 0x070;

// Device status bits
constexpr uint8_t STATUS_ACK       = 0x01;
constexpr uint8_t STATUS_DRIVER    = 0x02;
constexpr uint8_t STATUS_DRIVER_OK = 0x04;

constexpr uint32_t PAGE_SIZE = 4096;
constexpr uint16_t QUEUE_SIZE = 128;

constexpr uint16_t VIRTQ_DESC_F_WRITE = 2; // device writes into the buffer

// virtio-input event (8 bytes)
struct virtio_input_event {
    uint16_t type;
    uint16_t code;
    uint32_t value;
};

constexpr uint16_t EV_KEY = 0x01;
constexpr uint16_t KEY_LEFTSHIFT  = 42;
constexpr uint16_t KEY_RIGHTSHIFT = 54;

// Legacy virtio split ring (descriptor / available / used).
struct vring_desc {
    uint64_t addr;
    uint32_t len;
    uint16_t flags;
    uint16_t next;
};
struct vring_avail {
    uint16_t flags;
    uint16_t idx;
    uint16_t ring[QUEUE_SIZE];
};
struct vring_used_elem {
    uint32_t id;
    uint32_t len;
};
struct vring_used {
    uint16_t flags;
    uint16_t idx;
    vring_used_elem ring[QUEUE_SIZE];
};

// Legacy virtqueue layout (see virtio_ring.h "vring_size"):
//   desc  at offset 0
//   avail immediately after the descriptor table: offset QUEUE_SIZE*16
//   used  at the next align boundary after avail
// With QUEUE_SIZE=128 and a 4K align: avail@2048, used@4096.
constexpr uint16_t AVALI_OFF = QUEUE_SIZE * 16;
alignas(PAGE_SIZE) uint8_t ring_mem[3 * PAGE_SIZE];
vring_desc*  const desc  = reinterpret_cast<vring_desc*>(ring_mem);
vring_avail* const avail = reinterpret_cast<vring_avail*>(ring_mem + AVALI_OFF);
vring_used*  const used  = reinterpret_cast<vring_used*>(ring_mem + PAGE_SIZE);
uint16_t last_used = 0;
bool shift = false;

// One event buffer per descriptor.
alignas(16) virtio_input_event evbuf[QUEUE_SIZE];

volatile uint32_t* base = nullptr;

volatile uint32_t* reg(uint32_t off) {
    return reinterpret_cast<volatile uint32_t*>(reinterpret_cast<uintptr_t>(base) + off);
}

inline void mmio_fence() {
    asm volatile("fence rw,rw" ::: "memory");
}

void print_dec(uint32_t v) {
    char buf[12];
    int i = 0;
    do {
        buf[i++] = static_cast<char>('0' + (v % 10));
        v /= 10;
    } while (v);
    while (i--) uart::put_char(buf[i]);
}

// Linux input keycode -> ASCII (US layout).
char keycode_to_char(uint16_t code, bool shift) {
    switch (code) {
        case 2:  return shift ? '!' : '1';
        case 3:  return shift ? '@' : '2';
        case 4:  return shift ? '#' : '3';
        case 5:  return shift ? '$' : '4';
        case 6:  return shift ? '%' : '5';
        case 7:  return shift ? '^' : '6';
        case 8:  return shift ? '&' : '7';
        case 9:  return shift ? '*' : '8';
        case 10: return shift ? '(' : '9';
        case 11: return shift ? ')' : '0';
        case 12: return shift ? '_' : '-';
        case 13: return shift ? '+' : '=';
        case 14: return '\b';
        case 15: return '\t';
        case 16: return shift ? 'Q' : 'q';
        case 17: return shift ? 'W' : 'w';
        case 18: return shift ? 'E' : 'e';
        case 19: return shift ? 'R' : 'r';
        case 20: return shift ? 'T' : 't';
        case 21: return shift ? 'Y' : 'y';
        case 22: return shift ? 'U' : 'u';
        case 23: return shift ? 'I' : 'i';
        case 24: return shift ? 'O' : 'o';
        case 25: return shift ? 'P' : 'p';
        case 26: return shift ? '{' : '[';
        case 27: return shift ? '}' : ']';
        case 28: return '\n';
        case 30: return shift ? 'A' : 'a';
        case 31: return shift ? 'S' : 's';
        case 32: return shift ? 'D' : 'd';
        case 33: return shift ? 'F' : 'f';
        case 34: return shift ? 'G' : 'g';
        case 35: return shift ? 'H' : 'h';
        case 36: return shift ? 'J' : 'j';
        case 37: return shift ? 'K' : 'k';
        case 38: return shift ? 'L' : 'l';
        case 39: return shift ? ':' : ';';
        case 40: return shift ? '"' : '\'';
        case 41: return shift ? '~' : '`';
        case 43: return shift ? '|' : '\\';
        case 44: return shift ? 'Z' : 'z';
        case 45: return shift ? 'X' : 'x';
        case 46: return shift ? 'C' : 'c';
        case 47: return shift ? 'V' : 'v';
        case 48: return shift ? 'B' : 'b';
        case 49: return shift ? 'N' : 'n';
        case 50: return shift ? 'M' : 'm';
        case 51: return shift ? '<' : ',';
        case 52: return shift ? '>' : '.';
        case 53: return shift ? '?' : '/';
        case 57: return ' ';
        default: return 0;
    }
}

// Populate desc + avail ring so the device can hand us 128 event buffers.
void queue_setup() {
    for (uint16_t i = 0; i < QUEUE_SIZE; ++i) {
        desc[i].addr = reinterpret_cast<uintptr_t>(&evbuf[i]);
        desc[i].len = sizeof(virtio_input_event);
        desc[i].flags = VIRTQ_DESC_F_WRITE; // device writes into it
        desc[i].next = 0;
    }
    avail->flags = 0;
    avail->idx = 0;
    for (uint16_t i = 0; i < QUEUE_SIZE; ++i) {
        avail->ring[i] = i;
    }
    used->idx = 0;
    last_used = 0;
    mmio_fence();
    // Hand all 128 buffers to the device.
    avail->idx = QUEUE_SIZE;
    mmio_fence();
    *reg(R_QUEUE_NOTIFY) = 0; // notify queue 0 (eventq)
}

} // namespace

namespace virtio {

int keyboard_init() {
    // Scan virtio-mmio slots; QEMU "virt" maps them at 0x10001000 + i*0x1000.
    // The keyboard is the slot with device_id == VIRT_ID_INPUT.
    base = nullptr;
    for (unsigned i = 0; i < 16; ++i) {
        auto slot = reinterpret_cast<volatile uint32_t*>(VIRTIO_BASE + i * 0x1000);
        if (slot[R_MAGIC / 4] != VIRT_MAGIC)
            continue;
        if (slot[R_DEVICE_ID / 4] == VIRT_ID_INPUT) {
            base = slot;
            break;
        }
    }
    if (base == nullptr) {
        uart::put_string("keyboard: virtio-input not found\n");
        return -1;
    }
    uart::put_string("keyboard: virtio-input found at vaddr=");
    uart::put_char('0' + static_cast<char>((reinterpret_cast<uintptr_t>(base) - VIRTIO_BASE) / 0x1000));
    uart::put_char('\n');

    // Reset device.
    *reg(R_STATUS) = 0;
    // 1. Acknowledge, 2. Driver.
    *reg(R_STATUS) = STATUS_ACK | STATUS_DRIVER;
    // 3. We accept no optional features.
    *reg(R_DRV_FEATURES_SEL) = 0;
    *reg(R_DRV_FEATURES) = 0;
    mmio_fence();

    // Configure eventq (queue 0).
    *reg(R_GUEST_PAGE_SIZE) = PAGE_SIZE;
    *reg(R_QUEUE_SEL) = 0;
    if (*reg(R_QUEUE_NUM_MAX) < QUEUE_SIZE) {
        uart::put_string("keyboard: queue too small\n");
        return -1;
    }
    *reg(R_QUEUE_NUM) = QUEUE_SIZE;
    *reg(R_QUEUE_ALIGN) = PAGE_SIZE;
    // The ring lives in .bss right after the kernel; physical == virtual (MMU off).
    uintptr_t ring_paddr = reinterpret_cast<uintptr_t>(ring_mem);
    *reg(R_QUEUE_PFN) = static_cast<uint32_t>(ring_paddr / PAGE_SIZE);
    mmio_fence();

    // 4. Driver OK.
    *reg(R_STATUS) = STATUS_ACK | STATUS_DRIVER | STATUS_DRIVER_OK;
    mmio_fence();

    queue_setup();
    uart::put_string("keyboard: ready\n");
    return 0;
}

char keyboard_read() {
    mmio_fence();
    uint16_t used_idx = used->idx;
    if (used_idx == last_used)
        return 0;

    char out = 0;
    while (last_used != used_idx) {
        uint16_t id = used->ring[last_used % QUEUE_SIZE].id;
        virtio_input_event ev = evbuf[id];

        if (ev.type == EV_KEY) {
            if (ev.code == KEY_LEFTSHIFT || ev.code == KEY_RIGHTSHIFT) {
                ::shift = ev.value != 0;
            } else if (ev.value == 1) { // key press
                out = keycode_to_char(ev.code, ::shift);
            }
        }

        // Recycle this descriptor back to the device.
        avail->ring[avail->idx % QUEUE_SIZE] = id;
        avail->idx++;
        last_used++;
    }
    mmio_fence();
    *reg(R_QUEUE_NOTIFY) = 0;
    return out;
}

} // namespace virtio