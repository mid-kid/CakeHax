#pragma once

extern void *jump_table __asm__("jump_table");
extern void *jump_table_end __asm__("jump_table_end");
extern void *jt_return __asm__("jt_return");
extern void *jt_registers __asm__("jt_registers");
