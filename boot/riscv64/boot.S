/* RISC-V 64-bit boot stub.
 *
 * OpenSBI (fw_dynamic) boots this file as the next payload in S-mode.
 * On entry:
 *   a0 = hart id
 *   a1 = device tree blob (DTB) pointer
 * MMU is off, so virtual == physical addresses.
 */
.section .text.init, "ax", @progbits
.globl _start
.type _start, @function
_start:
    /* Zero the BSS section */
    la   t0, __bss_start
    la   t1, __bss_end
1:  bgeu t0, t1, 2f
    sd   zero, 0(t0)
    addi t0, t0, 8
    j    1b

2:
    /* Set up the stack pointer (grows down from __stack_top) */
    la   sp, __stack_top

    /* Jump into the C++ kernel (never returns) */
    call kernel_main

    /* Catch-all halt; kernel_main should never return */
3:  wfi
    j    3b

.size _start, . - _start