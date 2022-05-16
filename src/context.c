/*************************************************************************
    > File Name: context.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Sat 07 May 2022 06:50:03 AM UTC
 ************************************************************************/

#include <stdio.h>
#include "utils.h"
#include "context.h"
#include "sys_schema.h"
#include "schema.h"
#include "hashmap.h"
#include "hash.h"
#include "log.h"
#define SCHEMA_BUF_SIZE (496)
#define CONTEXT_SCHEMA_CACHE_COUNT (64)
const static char *del_kv_wal_log[] = {"del_sys.wal", "del_user.wal"};

static int *context_schema_traverse(void *k, void *v)
{

  return 0;
}
context *context_open(conf *cf)
{

  if (cf == NULL)
  {
    return NULL;
  }
  char *home = cf->db_home;
  if (!home)
  {
    return NULL;
  }
  if (access(home, R_OK) != 0)
  {
    mkdir(home, 0666);
  }
  context *ctx = (context *)calloc(1, sizeof(*ctx));
  assert(ctx != NULL);
  ctx->cf = cf;
  ctx->schema_cache = hashmap_alloc(CONTEXT_SCHEMA_CACHE_COUNT, (hashmap_hash_cb)&hash_fnv1_32, (hashmap_key_compare_cb)&memcmp);

  char wal_path[256] = {'\0'};
  snprintf((char *)&wal_path, 256, "%s/%s", home, del_kv_wal_log[SYS_WAL_LOG_INDEX]);
  ctx->sys_wal_log_fd = open((char *)&wal_path, O_RDWR | O_CREAT | O_TRUNC);

  size_t sys_schema_count = sizeof(sys_schema) / sizeof(sys_schema[0]);
  schema *meta_schema = NULL;
  for (size_t i = 0; i < sys_schema_count; i++)
  {
    char buf[SCHEMA_BUF_SIZE] = {'\0'};
    snprintf((char *)&buf, SCHEMA_BUF_SIZE, "%s/%s", home, sys_schema[i]);
    if (access((char *)&buf, F_OK) != 0)
    {
      mkdir((char *)&buf, 0666);
    }

    schema *sm = schema_alloc(home, sys_schema[i], cf, ctx->sys_wal_log_fd);
    int ret = hashmap_put(ctx->schema_cache, sys_schema[i], strlen(sys_schema[i]), &sm, sizeof(schema *));
    if (strncmp(sys_schema[i], "sys_schema", strlen(sys_schema[i])) == 0)
    {
      meta_schema = sm;
      ctx->meta_schema = meta_schema;
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
    schema *new_schema = schema_alloc(db_home, schema_name, ctx->cf, -1);
    assert(new_schema != NULL);
    hashmap_put(ctx->schema_cache, schema_name, key_len, &new_schema, sizeof(void *));
    char wal_path[256] = {'\0'};
    snprintf((char *)&wal_path, 256, "%s/%s/%s", db_home, schema_name, del_kv_wal_log[USER_WAL_LOG_INDEX]);
    new_schema->del_wal_fd = open((char *)&wal_path, O_RDWR | O_CREAT | O_TRUNC);
    if (ctx->meta_schema)
    {
      ret = schema_put_kv(ctx->meta_schema, schema_name, key_len + 1, new_schema->meta, new_schema->meta->len);
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
    schema_del_kv(ctx->meta_schema, schema_name, key_len);
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