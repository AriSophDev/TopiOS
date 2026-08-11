; TopiOS - ISR Stubs for x86-64
; Each stub pushes a dummy error code (if needed) and the vector number,
; then jumps to isr_common which saves registers and calls the C++ dispatcher.

bits 64

extern interrupt_dispatcher

; ── Macro for exceptions WITHOUT error code ──
%macro ISR_NOERR 1
global isr_stub_%1
isr_stub_%1:
    push 0              ; dummy error code
    push %1             ; vector number
    jmp isr_common
%endmacro

; ── Macro for exceptions WITH error code ──
%macro ISR_ERR 1
global isr_stub_%1
isr_stub_%1:
    push %1             ; vector number (error code already on stack)
    jmp isr_common
%endmacro

; ── Common handler: save registers, call C++, restore, iretq ──
isr_common:
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    cld                 ; System V ABI requires DF=0

    mov rdi, rsp        ; first argument = pointer to InterruptFrame
    call interrupt_dispatcher

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    add rsp, 16         ; remove vector + error_code
    iretq

; ── CPU Exceptions (0-31) ──
ISR_NOERR 0     ; Divide Error
ISR_NOERR 1     ; Debug
ISR_NOERR 2     ; NMI
ISR_NOERR 3     ; Breakpoint
ISR_NOERR 4     ; Overflow
ISR_NOERR 5     ; BOUND Range Exceeded
ISR_NOERR 6     ; Invalid Opcode
ISR_NOERR 7     ; Device Not Available
ISR_ERR   8     ; Double Fault
ISR_NOERR 9     ; Coprocessor Segment Overrun
ISR_ERR   10    ; Invalid TSS
ISR_ERR   11    ; Segment Not Present
ISR_ERR   12    ; Stack-Segment Fault
ISR_ERR   13    ; General Protection Fault
ISR_ERR   14    ; Page Fault
ISR_NOERR 15    ; Reserved
ISR_NOERR 16    ; x87 FPU Error
ISR_ERR   17    ; Alignment Check
ISR_NOERR 18    ; Machine Check
ISR_NOERR 19    ; SIMD Exception
ISR_NOERR 20    ; Virtualization Exception
ISR_NOERR 21    ; Control Protection Exception
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_ERR   30    ; Security Exception
ISR_NOERR 31    ; Reserved

; ── Hardware IRQs (32-47) ──
ISR_NOERR 32    ; IRQ0  - PIT Timer
ISR_NOERR 33    ; IRQ1  - Keyboard
ISR_NOERR 34    ; IRQ2  - Cascade
ISR_NOERR 35    ; IRQ3  - COM2
ISR_NOERR 36    ; IRQ4  - COM1
ISR_NOERR 37    ; IRQ5  - LPT2
ISR_NOERR 38    ; IRQ6  - Floppy
ISR_NOERR 39    ; IRQ7  - Spurious Master
ISR_NOERR 40    ; IRQ8  - RTC
ISR_NOERR 41    ; IRQ9  - ACPI
ISR_NOERR 42    ; IRQ10 - Open
ISR_NOERR 43    ; IRQ11 - Open
ISR_NOERR 44    ; IRQ12 - PS/2 Mouse
ISR_NOERR 45    ; IRQ13 - FPU
ISR_NOERR 46    ; IRQ14 - Primary ATA
ISR_NOERR 47    ; IRQ15 - Secondary ATA / Spurious Slave

; ── Stub address table (for C++ to get function pointers) ──
section .rodata
global isr_stub_table
isr_stub_table:
%assign i 0
%rep 48
    dq isr_stub_%+i
%assign i i+1
%endrep
