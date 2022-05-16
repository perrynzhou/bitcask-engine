/*************************************************************************
    > File Name: schema_meta.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Sat 14 May 2022 10:18:45 AM UTC
 ************************************************************************/

#ifndef _SCHEMA_META_H
#define _SCHEMA_META_H
#include "utils.h"
typedef struct schema_meta
{
  size_t len;
  _Atomic(uint64_t) bytes;
  _Atomic(uint64_t) kv_count;
  _Atomic(uint8_t) active;
  _Atomic(uint32_t) data_file_cnt;
  char name[0];
} schema_meta;
schema_meta *schema_meta_alloc(const char *name);
inline void schema_meta_modify_active(schema_meta *sm, bool flag)
{
  sm->active = flag ? 1 : 0;
}

inline void schema_meta_modify_bytes(schema_meta *sm, uint64_t bytes, bool flag)
{
  if (sm)
  {
    if (flag)
    {
      sm->bytes += bytes;
      sm->kv_count += 1;
    }
    else
    {
      sm->bytes -= bytes;
      sm->kv_count -= 1;
    }
  }
}
inline void schema_meta_incr_filecnt(schema_meta *sm)
{
  if (sm)
  {
    atomic_fetch_add(&sm->data_file_cnt,1);
  }
}
void schema_meta_destroy(schema_meta *m);
#endif
