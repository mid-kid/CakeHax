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

#include <stdint.h>
#include "firmcompat.h"
#include "appcompat.h"
#include "memchunkhax.h"
#include "firmlaunchax.h"

int load_file(char *dest, short unsigned int *path, uint32_t offset, uint32_t size)
{
    uint32_t file_handle[8] = {0};
    uint32_t bytes_read = 0;

    int result = fopen(&file_handle, path, 1);
    if (result != 0) {
        return 1;
    }
    file_handle[1] = offset;

    fread(&file_handle, &bytes_read, dest, size);

    return 0;
}

__attribute__((naked))
void arm11_kernel_code()
{
    __asm__ volatile ("clrex");

    // Reboot and load our arm9 payload in arm9 kernel mode
    firmlaunch_arm9hax();

    // We should never return here
    while (1) {};
}

void main()
{
    int result;

    // Some offsets differ per firmware
    result = set_firmware_offsets();
    if (result != 0) return;  // Unsupported firmware, bailing out.

    // Load the arm9 payload to memory
    // Spider has size restrictions to the Launcher, so we need to load the arm9
    //   payload separately.
    result = load_file((char *)(0x14000000 + APP_CFW_OFFSET),
                       APP_LAUNCHER_PATH, 0x20000, ARM9_PAYLOAD_MAXSIZE);
    if (result != 0) return;  // The user is probably an idiot, bailing out.

    // Now, we gain arm11 kernel mode
    memchunk_arm11hax(arm11_kernel_code);
}
