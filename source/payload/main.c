#include <stdint.h>
#include <stddef.h>
#include "fatfs/ff.h"
#include "fatfs/sdmmc/sdmmc.h"
#include "../launcher_path.h"

void *payload_loc = (void *)0x08000000;
unsigned int payload_offset = 0x22000;

// This is where the loader writes the framebuffer offsets
uint32_t *framebuffer_offsets = (uint32_t *)0x23FFFE00;

// This is supposed to be a small-ish payload that loads a bigger payload.
// This exists because some payloads are too big to be loaded in the memory of some apps.
void main()
{
    FATFS fs;
    FIL handle;
    unsigned int payload_size;
    unsigned int bytes_read;

    // Mount the SD card
    if (f_mount(&fs, "0:", 1) != FR_OK) return;

    // Load the payload
    if (f_open(&handle, "/" LAUNCHER_PATH, FA_READ) != FR_OK) return;
    payload_size = f_size(&handle) - payload_offset;
    if (f_lseek(&handle, payload_offset) != FR_OK) return;
    if (f_read(&handle, payload_loc, payload_size, &bytes_read) != FR_OK) return;
    if (f_close(&handle) != FR_OK) return;

    // Unmount the SD card
    if (f_mount(NULL, "0:", 1) != FR_OK) return;

    // This is mostly to adhere to the gateway "standard",
    //   so existing gateway payloads don't have to be modified.
    // Write the framebuffer offsets to the places where gateway expects them
    *(uint32_t *)0x080FFFC0 = framebuffer_offsets[0];  // Top screen left
    *(uint32_t *)0x080FFFC4 = framebuffer_offsets[1];  // Top screen right
    *(uint32_t *)0x080FFFD4 = framebuffer_offsets[2];  // Bottom screen
    *(uint32_t *)0x080FFFD8 = 0;  // Select top framebuffer?

    // Jump to the payload, right behind the interrupt vector table.
    ((void (*)())(payload_loc + 0x30))();
}
