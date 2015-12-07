/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef appcompat_h
#define appcompat_h

#include <stdint.h>
#include "launcher_path.h"

struct app_offsets
{
    uint32_t spec;

    int (*GSPGPU_FlushDataCache)(void *address, uint32_t length);
    void (*nn__gxlow__CTR__CmdReqQueueTx__TryEnqueue)(void *arg1, void *arg2);
    int (*fopen)(uint32_t (*handle)[], short unsigned int *path, int flags);
    int (*fread)(uint32_t (*handle)[], uint32_t *read, void *buffer, uint32_t size);

    uint32_t gpuHandle;
};

extern const struct app_offsets *app;
int set_app_offsets();

#if defined(ENTRY_MSET)
    // The usable area for this app
    #define APP_FCRAM_ADDR 0x14000000

    #define APP_CFW_OFFSET 0x400000
    #define APP_LAUNCHER_PATH (L"YS:/" LAUNCHER_PATH)

#elif defined(ENTRY_SPIDER)
    // The usable area for this app
    #define APP_FCRAM_ADDR 0x18400000

    #define APP_CFW_OFFSET 0x4410000
    #define APP_LAUNCHER_PATH (L"dmc:/" LAUNCHER_PATH)
#endif

// Locations in fcram
#define APP_CHECK_MEM (APP_FCRAM_ADDR + 0x1000)
#define APP_ARM11_BUFFER (APP_FCRAM_ADDR + 0x2000)
#define APP_MEM_HAX_MEM (APP_FCRAM_ADDR + 0x50000)

#endif
