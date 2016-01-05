#include <stdint.h>

void memcpy32(void *dest, const void *src, uint32_t size)
{
    uint32_t *dest32 = (uint32_t *)dest;
    uint32_t *src32 = (uint32_t *)src;
    for (uint32_t i = 0; i < size / 4; i++) {
        dest32[i] = src32[i];
    }
}

void memset(void *dest, int filler, uint32_t size)
{
    char *destc = (char *)dest;
    for (uint32_t i = 0; i < size; i++) {
        destc[i] = filler;
    }
}

void memcpy(void *dest, const void *src, uint32_t size)
{
    char *destc = (char *)dest;
    const char *srcc = (const char *)src;
    for (uint32_t i = 0; i < size; i++) {
        destc[i] = srcc[i];
    }
}
