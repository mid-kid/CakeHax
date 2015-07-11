@ Copyright (C) 2015 mid-kid
@
@ This file is part of CakeHax.
@
@ CakeHax is free software: you can redistribute it and/or modify
@ it under the terms of the GNU General Public License as published by
@ the Free Software Foundation, version 2.0
@
@ CakeHax is distributed in the hope that it will be useful,
@ but WITHOUT ANY WARRANTY; without even the implied warranty of
@ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
@ GNU General Public License for more details.
@
@ You should have received a copy of the GNU General Public License
@ along with CakeHax.  If not, see <http://www.gnu.org/licenses/>.

.global invalidate_data_cache
invalidate_data_cache:
    mov r0, #0
    mcr p15, 0, r0, c7, c14, 0 @ Clean and Invalidate Entire Data Cache
    mcr p15, 0, r0, c7, c10, 4 @ Data Synchronization Barrier
    bx lr

.global invalidate_instruction_cache
invalidate_instruction_cache:
    mov r0, #0
    mcr p15, 0, r0, c7, c5, 0
    mcr p15, 0, r0, c7, c5, 4
    mcr p15, 0, r0, c7, c5, 6
    mcr p15, 0, r0, c7, c10, 4
    bx lr

.global asm_memcpy
asm_memcpy:
    add r2, r1

    .memcpy_loop:
        ldmia r1!, {r3}
        stmia r0!, {r3}
        cmp r1, r2
        bcc .memcpy_loop

    bx lr
