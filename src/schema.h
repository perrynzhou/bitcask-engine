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
#include "schema_meta.h"

typedef struct schema
{
  char *data_home;
  schema_meta *meta;
  data_file **files;
  art_tree index_tree;
  _Atomic(uint32_t) fid;
  conf *cf;
  pthread_mutex_t lock;
} schema;

typedef int (*schema_load_cb)(void *ctx1,void *ctx2,void *data);
schema *schema_alloc(const char *db_home, const char *name, conf *cf);
int schema_add_data_file(schema *m, int fid);
int schema_drop_data_file(schema *m, int fid);


schema *schema_alloc_from_meta(schema_meta *m, conf *cf);
int schema_create(schema *m);
int schema_put_kv(schema *m, void *key, size_t key_sz, void *value, size_t value_sz);
void *schema_get_kv(schema *m, void *key, size_t key_sz);
int schema_del_kv(schema *m, void *key, size_t key_sz);
void schema_close(schema *m);
void schema_destroy(schema *m);
/*** load func ***/
schema *load_schema_meta_from_file(const char *path, void *ctx1, void *ctx2, schema_load_cb cb);
int load_schema_data_from_file(schema *sch, void *ctx1, void *ctx2, schema_load_cb cb);
#endif
