#include <stddef.h>
#include <stdint.h>
#include "arm11_tools.h"

void *memcpy32(void *dst, const void *src, size_t n)
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
