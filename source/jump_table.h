#ifndef jump_table_h
#define jump_table_h

extern void *jump_table __asm__("jump_table");
extern void *jump_table_size __asm__("jump_table_size");
extern void *jt_return __asm__("jt_return");
extern void *jt_regs __asm__("jt_regs");

#endif
