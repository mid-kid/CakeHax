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

#include "appcompat.h"

void (*memcpy)(void *dest, void *src, uint32_t len) = (void *)FUNC_MEMCPY;
int (*GSPGPU_FlushDataCache)(void *address, uint32_t length) = (void *)FUNC_GSPGPU_FLUSHDATACACHE;
void (*nn__gxlow__CTR__CmdReqQueueTx__TryEnqueue)(void *arg1, void *arg2) = (void *)FUNC_NN__GXLOW__CTR__CMDREQQUEUETX__TRYENQUEUE;
uint32_t (*svcControlMemory)(uint32_t *outaddr, uint32_t *addr0, uint32_t *addr1, uint32_t size, uint32_t operation, uint32_t permissions) = (void *)FUNC_SVCCONTROLMEMORY;
int (*fopen)(uint32_t (*handle)[], short unsigned int *path, int flags) = (void *)FUNC_FOPEN;
int (*fread)(uint32_t (*handle)[], uint32_t *read, void *buffer, uint32_t size) = (void *)FUNC_FREAD;
int (*fwrite)(uint32_t (*handle)[], uint32_t *written, void *src, uint32_t size) = (void *)FUNC_FWRITE;

#ifdef ENTRY_SPIDER
int (*GX_SetTextureCopy)(void *input_buffer, void *output_buffer, uint32_t size, int in_x, int in_y, int out_x, int out_y, int flags) = (void *)FUNC_GX_SETTEXTURECOPY;
int (*svcSleepThread)(unsigned long long nanoseconds) = (void *)FUNC_SVCSLEEPTHREAD;
#endif
