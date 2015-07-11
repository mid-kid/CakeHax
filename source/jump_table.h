/*
 * Copyright (C) 2015 mid-kid
 *
 * This file is part of CakeHax.
 *
 * CakeHax is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2.0
 *
 * CakeHax is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with CakeHax.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __jump_table_h__
#define __jump_table_h__

extern void *jump_table __asm__("jump_table");
extern void *jump_table_end __asm__("jump_table_end");
extern void *jt_return __asm__("jt_return");
extern void *jt_pdn_regs __asm__("jt_pdn_regs");
extern void *jt_pxi_regs __asm__("jt_pxi_regs");

#endif
