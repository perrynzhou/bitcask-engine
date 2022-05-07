/*************************************************************************
    > File Name: schema.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Sat 07 May 2022 06:50:49 AM UTC
 ************************************************************************/

#ifndef _SCHEMA_H
#define _SCHEMA_H
#include "utils.h"
#include <stdatomic.h>
typedef struct schema
{
  char *name;
  int fd;
  _Atomic(uint64_t) bytes;
  _Atomic(uint64_t) obj_count;
  
} schema;
schema *schema_alloc(const char *name);
void schema_destroy(schema *m);
inline int schema_modify(schema *m, uint64_t bytes)
{
  if (m)
  {
    atomic_fetch_add(&m->bytes, bytes);
    atomic_fetch_add(&m->obj_count, 1);
  }
  return -1;
}
#endif
