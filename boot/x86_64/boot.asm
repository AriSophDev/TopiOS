; TopiOS Boot - Multiboot2 header + 32→64 bit long mode transition

MB2_MAGIC      equ 0xE85250D6
MB2_ARCH       equ 0
MB2_FLAGS      equ 0
MB2_HDRLEN     equ header_end - header_start
MB2_CHECKSUM   equ -(MB2_MAGIC + MB2_ARCH + MB2_HDRLEN)

; ── Multiboot2 header ────────────────────────────────────────────
section .multiboot progbits alloc noexec nowrite align=8
header_start:
    dd MB2_MAGIC
    dd MB2_ARCH
    dd MB2_HDRLEN
    dd MB2_CHECKSUM
    ; end tag
    dw 0
    dw 0
    dd 8
header_end:


global _start
extern kernel_main

bits 32
_start:
    cli
    mov esp, stack_top

    ; Save multiboot info
    push ebx
    push eax

    ; ── Verify long mode support ──
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb .no_long_mode

    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz .no_long_mode

    ; ── Build identity-mapped page tables (first 1GB via 2MB pages) ──
    ; Zero out 3 pages (PML4 + PDP + PD)
    mov edi, pml4_table
    mov ecx, (4096 * 3) / 4
    xor eax, eax
.clear:
    mov [edi + ecx*4 - 4], eax
    dec ecx
    jnz .clear

    ; PML4[0] → PDP
    mov eax, pdp_table
    or eax, 0x03
    mov [pml4_table], eax
    mov dword [pml4_table + 4], 0

    ; PDP[0] → PD
    mov eax, pd_table
    or eax, 0x03
    mov [pdp_table], eax
    mov dword [pdp_table + 4], 0

    ; PD: 512 entries × 2MB = 1GB identity map
    mov edi, pd_table
    mov eax, 0x00000083          ; present | writable | page_size (2MB)
    mov ecx, 512
.fill_pd:
    mov [edi], eax
    mov dword [edi + 4], 0
    add eax, 0x00200000
    add edi, 8
    dec ecx
    jnz .fill_pd

    ; ── Enable PAE + long mode + paging ──
    mov eax, pml4_table
    mov cr3, eax

    mov eax, cr4
    or eax, 1 << 5               ; PAE
    mov cr4, eax

    mov ecx, 0xC0000080          ; EFER MSR
    rdmsr
    or eax, 1 << 8               ; long mode enable
    wrmsr

    mov eax, cr0
    or eax, (1 << 31) | (1 << 0) ; paging + protected mode
    mov cr0, eax

    ; ── Load 64-bit GDT and jump ──
    lgdt [gdt64_ptr]
    jmp 0x08:_long_mode_entry

.no_long_mode:
    mov edi, 0xB8000
    mov esi, err_msg
    mov ecx, err_len
    mov ah, 0x4F
.print:
    lodsb
    stosw
    dec ecx
    jnz .print
.halt:
    hlt
    jmp .halt

; ── 64-bit entry ─────────────────────────────────────────────────
bits 64

_long_mode_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    mov rsp, stack_top
    call kernel_main

.halt:
    cli
    hlt
    jmp .halt

; ── Data ─────────────────────────────────────────────────────────
section .data alloc nowrite align=8

gdt64:
    dq 0
.code_seg: equ $ - gdt64
    dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53)
.data_seg: equ $ - gdt64
    dq (1 << 44) | (1 << 47) | (1 << 41)

gdt64_ptr:
    dw gdt64_ptr - gdt64 - 1
    dq gdt64

err_msg: db "ERROR: No long mode support"
err_len: equ $ - err_msg

; ── BSS ──────────────────────────────────────────────────────────
section .bss nobits alloc noexec nowrite align=4096
align 4096
pml4_table:  resb 4096
pdp_table:   resb 4096
pd_table:    resb 4096
stack_bottom: resb 16384
stack_top:

