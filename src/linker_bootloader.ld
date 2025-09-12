OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS
{
    . = 0x7c00;

    .text : {
        *(.text)
        *(.text.*)
    }

    .rodata : { *(.rodata) }

    .data : { *(.data) }

    .bss : {
        *(COMMON)
        *(.bss)
    }
}
