/*************************************************************************
    > File Name: kv_index.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Thu 26 May 2022 09:50:43 AM UTC
 ************************************************************************/

#include <stdio.h>
#include "utils.h"
#include "kv_index.h"
int kv_index_init(kv_index *idx, uint32_t fid, int64_t offset, size_t size)
{
   int ret = -1;
   if (idx)
   {
      idx->fid = fid;
      idx->offset = offset;
      idx->size = size;
      idx->active = 1;
   }
   return ret;
}
kv_index *kv_index_alloc(uint32_t fid, int64_t offset, size_t size)
{
   kv_index *it = (kv_index *)calloc(1, sizeof(kv_index));
   assert(it != NULL);
   it->fid = fid;
   it->offset = offset;
   it->size = size;
   it->active = 1;
   return it;
}

void kv_index_destroy(kv_index *it)
{
   if (it)
   {
      free(it);
   }
}