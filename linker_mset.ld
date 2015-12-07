/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
