/*************************************************************************
    > File Name: schema.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Sat 07 May 2022 06:50:49 AM UTC
 ************************************************************************/

#ifndef _SCHEMA_H
#define _SCHEMA_H
#include "utils.h"
#include "bplustree.h"
#include <stdatomic.h>


typedef struct schema_meta {
  size_t len;
  _Atomic(uint64_t) bytes;
  _Atomic(uint64_t) obj_count;
  uint8_t  is_active;
  char name[0];
}schema_meta;
typedef struct schema
{
  schema_meta *meta;
  int fd;
  bplus_tree *data;
  
} schema;
schema *schema_alloc(const char *name);
int schema_put(schema *m,void *key,size_t key_sz,void *value,size_t value_sz);
void *schema_get(schema *m,void *key,size_t key_sz);
int schema_del(schema *m,void *key,size_t key_sz);
void schema_destroy(schema *m,bool is_drop);
inline int schema_modify(schema *m, uint64_t bytes)
{
  if (m)
  {
    atomic_fetch_add(&m->meta->bytes, bytes);
    atomic_fetch_add(&m->meta->obj_count, 1);
  }
  return -1;
}
#endif
