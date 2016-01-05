#pragma once

#include <stdint.h>

void invalidate_data_cache();
void invalidate_instruction_cache();
void svcSleepThread(uint64_t nanoseconds);
uint32_t svcControlMemory(uint32_t *outaddr, uint32_t *addr0, uint32_t *addr1, uint32_t size, uint32_t operation, uint32_t permissions);

void memcpy32(void *dest, const void *src, uint32_t size);
void memset(void *dest, int filler, uint32_t size);
void memcpy(void *dest, const void *src, uint32_t size);
