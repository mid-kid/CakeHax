ENTRY(_start)
SECTIONS
{
    . = 0x009D2000;
    .text.start : { *(.text.start) }
    .text : { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }

    . = 0x18420000;
    .bss : { *(.bss) }
}
