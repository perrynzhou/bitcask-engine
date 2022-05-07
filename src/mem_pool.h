/*
 * Copyright (c) 2014  Wu, Xingbo <wuxb45@gmail.com>
 *
 * All rights reserved. No warranty, explicit or implicit, provided.
 */

#ifndef D9C9F658_655C_46F4_AC1D_DEA3961AE56F
#define D9C9F658_655C_46F4_AC1D_DEA3961AE56F

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

struct mem_pool;

void *huge_alloc(const uint64_t cap);

void huge_free(void *const ptr, const uint64_t cap);

struct mem_pool *mem_pool_new(const size_t cap);

uint8_t *mem_pool_alloc(struct mem_pool *const p, const size_t cap);

void mem_pool_free(struct mem_pool *const p);

void mem_pool_show(struct mem_pool *const p);

#endif 
