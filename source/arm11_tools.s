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
    push {r0}
    ldr r0, [sp, #4]
    ldr r4, [sp, #8]
    svc 0x01
    pop {r2}
    str r1, [r2]
    bx lr

.global svcSleepThread
svcSleepThread:
    svc 0x0A
    bx lr
