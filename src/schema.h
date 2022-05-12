/*************************************************************************
    > File Name: schema.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Sat 07 May 2022 06:50:49 AM UTC
 ************************************************************************/

#ifndef _SCHEMA_H
#define _SCHEMA_H
#include "utils.h"
#include "art.h"
#include "data_file.h"
#include "conf.h"
#include <stdatomic.h>
typedef struct schema_meta
{
  size_t len;
  _Atomic(uint64_t) bytes;
  _Atomic(uint64_t) kv_count;
  _Atomic(uint8_t)  active;
  _Atomic(uint32_t) data_file_cnt;
  char name[0];
} schema_meta;
typedef struct schema
{
  char *db_home;
  schema_meta *meta;
  _Atomic(uint32_t) data_file_id;
  data_file *cur_file;
  art_tree index_tree;
  conf *cf;
  pthread_mutex_t lock;
} schema;
schema *schema_alloc(const char *db_home, const char *name, conf *cf);
int schema_put(schema *m, void *key, size_t key_sz, void *value, size_t value_sz);
void *schema_get(schema *m, void *key, size_t key_sz);
int schema_del(schema *m, void *key, size_t key_sz);
void schema_close(schema *m);
void schema_destroy(schema *m);
#endif
