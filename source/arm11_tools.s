.global asm_memcpy
asm_memcpy:
    add r2, r1

    .memcpy_loop:
        ldmia r1!, {r3}
        stmia r0!, {r3}
        cmp r1, r2
        bcc .memcpy_loop

    bx lr
