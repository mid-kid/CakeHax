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

.global svcControlMemory
svcControlMemory:
    push {r0, r4}
    ldr r0, [sp, #8]
    ldr r4, [sp, #12]
    svc 0x01
    ldr r2, [sp], #4
    str r1, [r2]
    ldr r4, [sp], #4
    bx lr

.global svcSleepThread
svcSleepThread:
    svc 0x0A
    bx lr
