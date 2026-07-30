section .multiboot
align 8

mb2_header:
    dd 0xE85250D6
    dd 0
    dd mb2_end - mb2_header
    dd -(0xE85250D6 + 0 + (mb2_end - mb2_header))

    dw 0
    dw 0
    dd 8
mb2_end

section .bss
align 16
stack_bottom:
    resb 16384
stack_top:

section .text
global _start
extern kernel_main

_start:
    ; GRUB entra en 32 bits, así que usa ESP aquí
    mov esp, stack_top

    call kernel_main

.hang:
    cli
    hlt
    jmp .hang
