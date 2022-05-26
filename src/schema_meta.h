/*************************************************************************
    > File Name: schema_meta.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Sat 14 May 2022 10:18:45 AM UTC
 ************************************************************************/

#ifndef _SCHEMA_META_H
#define _SCHEMA_META_H
#include "utils.h"
#include "array.h"
#define MAX_SCHEMA_NAME_SIZE  (128)
typedef struct schema_meta
{
  _Atomic(uint64_t) bytes;
  _Atomic(uint64_t) kv_count;
  // active stand for kv is delete
  _Atomic(uint8_t)  active;
  _Atomic(uint32_t) file_cnt;
  char name[MAX_SCHEMA_NAME_SIZE];
} schema_meta;
schema_meta *schema_meta_alloc();
int schema_meta_init(schema_meta *m,uint64_t bytes,uint64_t count,uint8_t active,uint32_t file_cnt,const char *name);
int   schema_meta_save(schema_meta *meta,int fd);
void schema_meta_destroy(schema_meta *m);
#endif
