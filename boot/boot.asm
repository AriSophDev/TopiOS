section .multiboot
align 8

mb2_header:
    dd 0xE85250D6          ; Magic
    dd 0                   ; Architecture (0 = i386/x86)
    dd mb2_end - mb2_header
    dd -(0xE85250D6 + 0 + (mb2_end - mb2_header))

    dw 0                   ; End tag type
    dw 0                   ; Flags
    dd 8                   ; Size

mb2_end:


global _start
extern kernel_main

section .text

_start:
    call kernel_main

.hang:
    cli
    hlt
    jmp .hang
