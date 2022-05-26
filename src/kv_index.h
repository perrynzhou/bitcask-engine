/*************************************************************************
    > File Name: kv_index.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Thu 12 May 2022 02:46:15 AM UTC
 ************************************************************************/

#ifndef _KV_INDEX_H
#define _KV_INDEX_H
#include "utils.h"
typedef struct kv_index
{
   uint32_t fid;
   uint64_t offset;
   size_t size;
   uint8_t active : 1;
} kv_index;
int kv_index_init(kv_index *idx, uint32_t fid, int64_t offset, size_t size);
kv_index *kv_index_alloc(uint32_t fid, int64_t offset, size_t size);
void kv_index_destroy(kv_index *it);
#endif
