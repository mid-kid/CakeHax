#ifndef __jump_table_h__
#define __jump_table_h__

extern void *jump_table __asm__("jump_table");
extern void *jump_table_size __asm__("jump_table_size");
extern void *jt_return __asm__("jt_return");
extern void *jt_regs __asm__("jt_regs");

#endif
