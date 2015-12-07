/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "appcompat.h"

const struct app_offsets *app;

static const struct app_offsets apps[] =
{
#ifdef ENTRY_MSET
    {
        // 4.x
        .spec = 0xEB0312BF,

        .GSPGPU_FlushDataCache = (void *)0x001346C4,
        .nn__gxlow__CTR__CmdReqQueueTx__TryEnqueue = (void *)0x001AC924,
        .fopen = (void *)0x001B82A8,
        .fread = (void *)0x001B3954,

        .gpuHandle = (0x0027C580 + 0x58)
    }, {
        // 6.x
        .spec = 0xEB0334CF,

        .GSPGPU_FlushDataCache = (void *)0x00134A84,
        .nn__gxlow__CTR__CmdReqQueueTx__TryEnqueue = (void *)0x001B4E8C,
        .fopen = (void *)0x001C08B4,
        .fread = (void *)0x001BC188,

        .gpuHandle = (0x0028A580 + 0x58)
    }
#elif defined(ENTRY_SPIDER)
    {
        // 4.x
        .spec = 0xEB0676B5,

        .GSPGPU_FlushDataCache = (void *)0x00344B84,
        .nn__gxlow__CTR__CmdReqQueueTx__TryEnqueue = (void *)0x002CF3EC,
        .fopen = (void *)0x0025B0A4,
        .fread = (void *)0x002FC8E4,

        .gpuHandle = (0x003F54E8 + 0x58)
    }, {
        // 5.x
        .spec = 0xEB050B2A,

        .GSPGPU_FlushDataCache = (void *)0x001914FC,
        .nn__gxlow__CTR__CmdReqQueueTx__TryEnqueue = (void *)0x0012BF4C,
        .fopen = (void *)0x0022FE44,
        .fread = (void *)0x001686C0,

        .gpuHandle = (0x003D7C40 + 0x58)
    }, {
        // 9.x
        .spec = 0xEB050B28,

        .GSPGPU_FlushDataCache = (void *)0x00191504,
        .nn__gxlow__CTR__CmdReqQueueTx__TryEnqueue = (void *)0x0012BF04,
        .fopen = (void *)0x0022FE08,
        .fread = (void *)0x001686DC,

        .gpuHandle = (0x003D7C40 + 0x58)
    }, {
        // 4.2 CN
        .spec = 0xEB050466,

        .GSPGPU_FlushDataCache = (void *)0x00190118,
        .nn__gxlow__CTR__CmdReqQueueTx__TryEnqueue = (void *)0x0012BA40,
        .fopen = (void *)0x0022E334,
        .fread = (void *)0x001674BC,

        .gpuHandle = (0x003D6C40 + 0x58)
    }, {
        // 4.5 CN
        .spec = 0xEB05043C,

        .GSPGPU_FlushDataCache = (void *)0x0018FC0C,
        .nn__gxlow__CTR__CmdReqQueueTx__TryEnqueue = (void *)0x0012BA54,
        .fopen = (void *)0x0022E2B0,
        .fread = (void *)0x00166FC8,

        .gpuHandle = (0x003D6C40 + 0x58)
    }, {
        // 5.x CN
        .spec = 0xEB050657,

        .GSPGPU_FlushDataCache = (void *)0x001902A8,
        .nn__gxlow__CTR__CmdReqQueueTx__TryEnqueue = (void *)0x0012BA6C,
        .fopen = (void *)0x0022EA5C,
        .fread = (void *)0x0016751C,

        .gpuHandle = (0x003D7C40 + 0x58)
    }, {
        // 9.x CN
        .spec = 0xEB050656,

        .GSPGPU_FlushDataCache = (void *)0x001902B8,
        .nn__gxlow__CTR__CmdReqQueueTx__TryEnqueue = (void *)0x0012BA24,
        .fopen = (void *)0x0022EA24,
        .fread = (void *)0x00167540,

        .gpuHandle = (0x003D7C40 + 0x58)
    }, {
        // 4.x KR
        .spec = 0xEB050838,

        .GSPGPU_FlushDataCache = (void *)0x00190D30,
        .nn__gxlow__CTR__CmdReqQueueTx__TryEnqueue = (void *)0x0012BA14,
        .fopen = (void *)0x0022F284,
        .fread = (void *)0x001680F8,

        .gpuHandle = (0x003D7C40 + 0x58)
    }, {
        // 5.x KR
        .spec = 0xEB050A4B,

        .GSPGPU_FlushDataCache = (void *)0x0019154C,
        .nn__gxlow__CTR__CmdReqQueueTx__TryEnqueue = (void *)0x0012BA40,
        .fopen = (void *)0x0022FAC8,
        .fread = (void *)0x001686FC,

        .gpuHandle = (0x003D7C40 + 0x58)
    }, {
        // 9.x KR
        .spec = 0xEB050A49,

        .GSPGPU_FlushDataCache = (void *)0x00191554,
        .nn__gxlow__CTR__CmdReqQueueTx__TryEnqueue = (void *)0x0012B9F8,
        .fopen = (void *)0x0022FA8C,
        .fread = (void *)0x00168718,

        .gpuHandle = (0x003D7C40 + 0x58)
    }, {
        // 4.x TW
        .spec = 0xEB05084D,

        .GSPGPU_FlushDataCache = (void *)0x00190D34,
        .nn__gxlow__CTR__CmdReqQueueTx__TryEnqueue = (void *)0x0012BA40,
        .fopen = (void *)0x0022F2D8,
        .fread = (void *)0x001680FC,

        .gpuHandle = (0x003D7C40 + 0x58)
    }, {
        // 5.x TW
        .spec = 0xEB050A70,

        .GSPGPU_FlushDataCache = (void *)0x00191594,
        .nn__gxlow__CTR__CmdReqQueueTx__TryEnqueue = (void *)0x0012BA6C,
        .fopen = (void *)0x0022FB5C,
        .fread = (void *)0x00168744,

        .gpuHandle = (0x003D7C40 + 0x58)
    }, {
        // 9.x TW
        .spec = 0xEB050A6E,

        .GSPGPU_FlushDataCache = (void *)0x0019159C,
        .nn__gxlow__CTR__CmdReqQueueTx__TryEnqueue = (void *)0x0012BA24,
        .fopen = (void *)0x0022FB20,
        .fread = (void *)0x00168760,

        .gpuHandle = (0x003D7C40 + 0x58)
    }
#endif
};

int set_app_offsets()
{
    const struct app_offsets *cur, *btm;

    cur = apps;
    for (btm = cur + sizeof(apps) / sizeof(struct app_offsets); cur != btm; cur++)
        if (cur->spec == *(uint32_t *)0x0010000C) {
            app = cur;
            return 0;
        }

    return 1;
}
