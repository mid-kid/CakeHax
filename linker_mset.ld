ENTRY(_start)
SECTIONS
{
    . = 0x00240000;
    .text.start : { *(.text.start) }
    .text : { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }

    . = 0x14020000;
    .bss : { *(.bss) }
}
