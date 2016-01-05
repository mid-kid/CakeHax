#include <stdint.h>
#include "firmcompat.h"
#include "appcompat.h"
#include "memchunkhax.h"
#include "firmlaunchax.h"

static int load_file(char *dest, short unsigned int *path, uint32_t offset, uint32_t size)
{
    uint32_t file_handle[8] = {0};  // This optimizes to memset
    uint32_t bytes_read = 0;

    int result = app->fopen(&file_handle, path, 1);
    if (result != 0) return result;
    file_handle[1] = offset;

    app->fread(&file_handle, &bytes_read, dest, size);

    return 0;
}

__attribute__((naked))
static void arm11_kernel_code()
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

    // Some offsets that differ per entry
    result = set_app_offsets();
    if (result != 0) return;

    // Some offsets differ per firmware
    result = set_firmware_offsets();
    if (result != 0) return;  // Unsupported firmware, bailing out.

    // Load the arm9 payload to memory
    // Spider has size restrictions to the Launcher, so we need to load the arm9
    //   payload separately.
    result = load_file((char *)(0x14000000 + APP_CFW_OFFSET),
                       APP_LAUNCHER_PATH, 0x12000, ARM9_PAYLOAD_MAXSIZE);
    if (result != 0) return;  // The user is probably an idiot, bailing out.

    // Now, we gain arm11 kernel mode
    memchunk_arm11hax(arm11_kernel_code);
}
