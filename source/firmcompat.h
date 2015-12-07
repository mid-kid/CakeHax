/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef firmcompat_h
#define firmcompat_h

#include <stdint.h>
#include "appcompat.h"

struct firmware_offsets {
    uint32_t ver;

    uint32_t kernel_patch_address;
    uint32_t reboot_patch_address;
    uint32_t reboot_func_address;
    uint32_t jump_table_address;
    uint32_t fcram_address;
    uint32_t func_patch_address;
    uint32_t func_patch_return;
    uint32_t regs;
};

extern const struct firmware_offsets *fw;

int set_firmware_offsets();

#endif
