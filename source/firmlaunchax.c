/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "firmlaunchax.h"

#include <stddef.h>
#include <stdint.h>
#include "firmcompat.h"
#include "appcompat.h"

#if PAYLOAD_ARM9_SIZE > 0x10000
#error PAYLOAD_ARM9_SIZE is too large. This is the restriction of hooks.
#endif

static void clean(uintptr_t p)
{
    __asm__ volatile ("mcr p15, 0, %0, c7, c10, 1" :: "r"(p));
}

static void dsb()
{
    __asm__ volatile ("mcr p15, 0, %0, c7, c10, 4" :: "r"(0));
}

static void *memcpy32(void *dst, const void *src, size_t n)
{
    const uint32_t *src32;
    uint32_t *dst32;

    src32 = src;
    for (dst32 = dst; (uintptr_t)dst32 < (uintptr_t)dst + n; dst32++) {
        *dst32 = *src32;
        src32++;
    }

    return dst;
}

static void setup_gpu()
{
    volatile uint32_t *top_left1 = (volatile uint32_t *)(fw->regs + 0x468);
    volatile uint32_t *top_left2 = (volatile uint32_t *)(fw->regs + 0x46C);
    volatile uint32_t *top_right1 = (volatile uint32_t *)(fw->regs + 0x494);
    volatile uint32_t *top_right2 = (volatile uint32_t *)(fw->regs + 0x498);
    volatile uint32_t *top_selected = (volatile uint32_t *)(fw->regs + 0x478);
    volatile uint32_t *bottom1 = (volatile uint32_t *)(fw->regs + 0x568);
    volatile uint32_t *bottom2 = (volatile uint32_t *)(fw->regs + 0x56C);
    volatile uint32_t *bottom_selected = (volatile uint32_t *)(fw->regs + 0x578);

    uint32_t *save = (uint32_t *)(fw->fcram_address + 0x3FFFE00);
    if (*top_selected) {
        save[0] = *top_left2;
        save[1] = *top_right2;
    } else {
        save[0] = *top_left1;
        save[1] = *top_right1;
    }

    if (*bottom_selected) {
        save[2] = *bottom2;
    } else {
        save[2] = *bottom1;
    }
}

void firmlaunch_arm9hax()
{
    uintptr_t p, payload;

    payload = fw->fcram_address + APP_CFW_OFFSET;
    // Copy arm9 code
    uint32_t code_offset = 0x3F00000;
    memcpy32((void *)(fw->fcram_address + code_offset),
             (void *)(payload + PAYLOAD_TABLE_SIZE),
             PAYLOAD_ARM9_SIZE);

    // Prepare framebuffer info for arm9
    setup_gpu();

    // Copy the jump table
    memcpy32((void *)fw->jump_table_address, (void *)payload, PAYLOAD_TABLE_SIZE);

    // Write firmware-specific offsets to the jump table
    *(uint32_t *)(fw->jump_table_address + PAYLOAD_TABLE_SIZE) = fw->func_patch_return;
    *(uint32_t *)(fw->jump_table_address + PAYLOAD_TABLE_SIZE + 4) = fw->regs;

    // Patch arm11 functions
    *(uint32_t *)fw->func_patch_address = 0xE51FF004;
    *(uint32_t *)(fw->func_patch_address + 4) = 0xFFFF0C80;
    *(uint32_t *)fw->reboot_patch_address = 0xE51FF004;
    *(uint32_t *)(fw->reboot_patch_address + 4) = 0x1FFF4C80+4;

    for (p = fw->jump_table_address;
         p < fw->jump_table_address + PAYLOAD_TABLE_SIZE;
         p += 32)
    {
        clean(p);
    }

    clean(fw->func_patch_address);
    clean(fw->reboot_patch_address);
    dsb();

    // Trigger reboot
    ((void (*)())fw->reboot_func_address)(0, 0, 2, 0);

    while (1) {};
}
