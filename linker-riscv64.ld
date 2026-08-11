ENTRY(_start)

SECTIONS
{
    /* OpenSBI jumps to the kernel payload at 0x80200000 */
    . = 0x80200000;

    .text : ALIGN(4K)
    {
        *(.text.init)
        *(.text)
        *(.text.*)
    }

    .rodata : ALIGN(4K)
    {
        *(.rodata)
        *(.rodata.*)
    }

    .data : ALIGN(4K)
    {
        *(.data)
        *(.data.*)
    }

    .bss : ALIGN(4K)
    {
        __bss_start = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        __bss_end = .;
    }

    /* Reserve 16 KiB for the boot stack, laid out right after the BSS */
    . = ALIGN(16);
    __stack_bottom = .;
    . += 0x4000;
    __stack_top = .;

    /DISCARD/ :
    {
        /* Not valid on a bare-metal RISC-V ELF */
        *(.eh_frame)
        *(.comment)
    }
}