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

.section .text.start
.align 4
.global _start
_start:
    b main
