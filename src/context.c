/*************************************************************************
    > File Name: context.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Sat 07 May 2022 06:50:03 AM UTC
 ************************************************************************/

#include <stdio.h>
#include "utils.h"
#include "art.h"
#include "context.h"
#include "meta.h"
#include "schema.h"
#include "hashmap.h"
#include "hash.h"
#include "log.h"
#include "kv_index.h"
#include "kv_value.h"
#define SCHEMA_BUF_SIZE (4096)
#define CONTEXT_SCHEMA_CACHE_COUNT (64)
#define MAX_DATA_FILE_COUNT (65535)
static int *context_schema_traverse(void *k, void *v)
{

  return 0;
}

static inline schema *context_fetch_meta_schema(context *ctx)
{
  schema **meta_schema = NULL;
  if (ctx && ctx->schema_cache)
  {
    meta_schema = (schema **)hashmap_get(ctx->schema_cache, sys_schema[SYS_SCHEMA_META_INDEX], strlen(sys_schema[SYS_SCHEMA_META_INDEX]));
  }
  return *meta_schema;
}
int context_schema_traverse_cb(void *arg)
{
  char *v = (char *)arg;
  if (v != NULL)
  {
    slog_info("array_schema_traverse_cb:schema_name=%s\n", v);
    free(v);
    v = NULL;
  }
  return 0;
}

inline static context *context_alloc(conf *cf)
{
  context *ctx = (context *)calloc(1, sizeof(*ctx));
  assert(ctx != NULL);
  ctx->cf = cf;
  ctx->schema_cache = hashmap_alloc(CONTEXT_SCHEMA_CACHE_COUNT, (hashmap_hash_cb)&hash_fnv1_32, (hashmap_key_compare_cb)&memcmp);
  return ctx;
}
int load_schema_cb(void *ctx1, void *ctx2, void *data)
{
  int ret = -1;
  if (ctx1 && ctx2 && data)
  {
    array *arr = (array *)ctx1;
    schema_meta *meta = (schema_meta *)data;
    slog_info("load %s from disk,kv_count=%d", meta->name, meta->active);
    conf *cf = (conf *)ctx2;
    schema *s = schema_alloc_from_meta(meta, cf);
    ret = array_push_back(arr, s);
  }
  return ret;
}
context *context_open(conf *cf)
{

  context *ctx = context_alloc(cf);
  assert(ctx);

  if (access(cf->db_home, F_OK) != 0)
  {
    mkdir(cf->db_home, 0755);
  }
  char tmp_buf[SCHEMA_BUF_SIZE] = {'\0'};
  snprintf((char *)&tmp_buf, SCHEMA_BUF_SIZE, "%s/%s", cf->db_home, sys_schema[SYS_SCHEMA_META_INDEX]);

  if (access((char *)&tmp_buf, F_OK) != 0)
  {
    // init data_home path
    mkdir((char *)&tmp_buf, 0666);
  }

  snprintf((char *)&tmp_buf, SCHEMA_BUF_SIZE, "%s/%s/%09d.data", cf->db_home, sys_schema[SYS_SCHEMA_META_INDEX], 0);
  struct stat st;
  if (stat((char *)&tmp_buf, &st) != 0)
  {
    slog_info("create database home:%s", (char *)&tmp_buf);
    schema *s = schema_alloc(cf->db_home, sys_schema[SYS_SCHEMA_META_INDEX], cf);
    schema_add_data_file(s, 0);
    hashmap_put(ctx->schema_cache, sys_schema[SYS_SCHEMA_META_INDEX], strlen(sys_schema[SYS_SCHEMA_META_INDEX]), &s, sizeof(void **));
    slog_info("add %s to cache", s->meta->name);
  }
  else
  {
    array *arr = array_alloc(4, true);
    schema *s_meta = schema_load_from_file((char *)&tmp_buf, arr, cf, load_schema_cb);
    hashmap_put(ctx->schema_cache, (char *)&s_meta->meta->name, strlen((char *)&s_meta->meta->name), &s_meta, sizeof(void **));
    if (array_len(arr) > 0)
    {
      for (size_t i = 0; i < array_len(arr); i++)
      {
        schema *s = (schema *)array_value(arr, i);
        char *schema_name = (char *)&s->meta->name;
        hashmap_put(ctx->schema_cache, schema_name, strlen(schema_name), &s, sizeof(void **));
        slog_info("load %s from disk,kv_count=%d", s->meta->name, s->meta->active);
      }
    }
  }
  return ctx;
}
void *context_get_schema(context *ctx, char *schema_name)
{
  void *ptr = NULL;
  if (ctx && ctx->schema_cache)
  {
    schema **sm = (schema **)hashmap_get(ctx->schema_cache, schema_name, strlen(schema_name));
    ptr = *sm;
  }
  return ptr;
}
int context_put_schema(context *ctx, char *schema_name)
{
  int ret = -1;
  if (ctx && schema_name)
  {
    char *db_home = ctx->cf->db_home;

    size_t key_len = strlen(schema_name);
    void *found = hashmap_get(ctx->schema_cache, schema_name, key_len);
    if (found)
    {
      return ret;
    }
    // add schema to cache map
    schema *new_schema = schema_alloc(db_home, schema_name, ctx->cf);
    assert(new_schema != NULL);
    schema_add_data_file(new_schema, 0);
    hashmap_put(ctx->schema_cache, schema_name, key_len, &new_schema, sizeof(void **));
    schema *s = context_fetch_meta_schema(ctx);
    if (s)
    {
      ret = schema_put_kv(s, schema_name, key_len + 1, new_schema->meta, sizeof(schema_meta));
    }
  }
  return ret;
}
int context_del_schema(context *ctx, char *schema_name)
{
  int ret = -1;
  if (ctx && schema_name)
  {

    size_t key_len = strlen(schema_name);
    hashmap_del(ctx->schema_cache, schema_name, key_len);
    schema *s = (schema *)hashmap_get(ctx->schema_cache, sys_schema[SYS_SCHEMA_META_INDEX], strlen(sys_schema[SYS_SCHEMA_META_INDEX]));
    if (s)
    {
      schema *current_schema = (schema *)hashmap_get(ctx->schema_cache, schema_name, strlen(schema_name));
      ret = schema_put_kv(s, schema_name, key_len + 1, current_schema->meta, sizeof(schema_meta));
    }
    ret = 0;
  }
  return ret;
}

void context_close(context *ctx)
{
  // hashmap_traverse
  if (ctx)
  {
    hashmap_traverse(ctx->schema_cache, (hashmap_traverse_cb)&context_schema_traverse);
    hashmap_traverse(ctx->schema_cache, (hashmap_traverse_cb)&context_schema_traverse);
  }
}