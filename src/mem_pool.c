/*
 * Copyright (c) 2014  Wu, Xingbo <wuxb45@gmail.com>
 *
 * All rights reserved. No warranty, explicit or implicit, provided.
 */

#define _GNU_SOURCE
#define _LARGEFILE64_SOURCE

#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "mem_pool.h"
#define MEMPOOL_UNIT (1024 * 1024 * 2)

struct mem_pool {
  bool using_mmap;
  uint64_t pos;
  uint64_t max;
  uint8_t *space;
};

static const bool USING_MALLOC = false;

void *huge_alloc(const uint64_t cap) {
  const size_t hcap = ((cap + MEMPOOL_UNIT - 1) / MEMPOOL_UNIT) * MEMPOOL_UNIT;
  if (hcap != cap)
    return NULL;
  if (USING_MALLOC) {
    return aligned_alloc(MEMPOOL_UNIT, cap);
  } else {
    void *const m = mmap(NULL, cap, PROT_READ | PROT_WRITE,
                         MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
    if (m == MAP_FAILED) {
      void *const m2 = mmap(NULL, cap, PROT_READ | PROT_WRITE,
                            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
      if (m2 == MAP_FAILED) {
        return NULL;
      } else {
        return m2;
      }
    } else {
      return m;
    }
  }
}

void huge_free(void *const ptr, const uint64_t cap) {
  if (USING_MALLOC) {
    free(ptr);
  } else {
    munmap(ptr, cap);
  }
}

static bool space_alloc_mmap(struct mem_pool *const mempool, const size_t cap) {
  const size_t hcap = ((cap + MEMPOOL_UNIT - 1) / MEMPOOL_UNIT) * MEMPOOL_UNIT;
  assert(hcap >= cap);
  void *const m = huge_alloc(hcap);
  // MAP_UNINITIALIZED need kernel configuration:
  // CONFIG_MMAP_ALLOW_UNINITIALIZED. it's unsafe
  if (m) {
    mempool->using_mmap = true;
    mempool->space = (uint8_t *)m;
    mempool->max = (uint64_t)hcap;
    return true;
  } else {
    mempool->using_mmap = false;
    mempool->space = (uint8_t *)malloc(cap);
    mempool->max = (uint64_t)cap;
    return (mempool->space) ? true : false;
  }
}

struct mem_pool *mem_pool_new(const size_t cap) {
  struct mem_pool *const p = (struct mem_pool *)malloc(sizeof(*p));
  if (p == NULL) {
    return NULL;
  }

  p->pos = 0;
  const bool r = space_alloc_mmap(p, cap);
  if (r == false) {
    free(p);
    return NULL;
  }
  return p;
}

uint8_t *mem_pool_alloc(struct mem_pool *const p, const size_t cap) {
  const size_t hcap = ((cap + 8u) & (~7u));
  if ((p == NULL) || ((p->pos + hcap) > p->max)) {
    logi("mem_pool_alloc() failed(1): pos: %lu, cap: %zu, max: %lu\n",
         p ? p->pos : ~0, cap, p ? p->max : ~0);
    fflush(stderr);
    return NULL;
  }
  const uint64_t pos = __sync_fetch_and_add(&(p->pos), hcap);
  if ((pos + hcap) > p->max) {
    logi("mem_pool_alloc() failed(2): pos: %lu, cap: %zu, max: %lu\n", p->pos,
         cap, p->max);
    fflush(stderr);
    return NULL;
  }
  uint8_t *const r = p->space + pos;
  return r;
}

void mem_pool_free(struct mem_pool *const p) {
  if (p->using_mmap) {
    huge_free(p->space, p->max);
  } else {
    free(p->space);
  }
  free(p);
}

void mem_pool_show(struct mem_pool *const p) {
  logi("mempool: pos: %lu, max: %lu\n", p->pos, p->max);
}
