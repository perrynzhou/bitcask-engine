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
#define CONTEXT_SYSSCHEMA_INSIDE_COUNT (64)
#define CONTEXT_USERSCHEMA_INSIDE_COUNT (65535)
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
  ctx->sys_map = hashmap_alloc(CONTEXT_SYSSCHEMA_INSIDE_COUNT, (hashmap_hash_cb)&hash_fnv1_32, (hashmap_key_compare_cb)&memcmp);
  ctx->user_map = hashmap_alloc(CONTEXT_USERSCHEMA_INSIDE_COUNT, (hashmap_hash_cb)&hash_fnv1_32, (hashmap_key_compare_cb)&memcmp);

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
    int ret = hashmap_put(ctx->sys_map, sys_schema[i], strlen(sys_schema[i]), sm, sizeof(schema *));
    logi("load %s schema,ret=%d", (char *)&buf, ret);
    if (strncmp(sys_schema[i], "sys_schema", strlen(sys_schema[i])) == 0)
    {
      ctx->meta_schema = meta_schema;
    }
  }
  return ctx;
}
void *context_get_schema(context *ctx, char *schema_name, bool flag)
{
  void *schema = NULL;
  if (ctx && schema)
  {
    hashmap *mp = (flag) ? ctx->sys_map : ctx->user_map;
    if (mp)
    {
      schema = hashmap_get(mp, schema_name, strlen(schema_name));
    }
  }
  return schema;
}
int context_put_schema(context *ctx, char *schema_name)
{
  int ret = -1;
  if (ctx && schema_name)
  {
    char *db_home = ctx->cf->db_home;

    size_t key_len = strlen(schema_name);
    void *found = hashmap_get(ctx->user_map, schema_name, key_len);
    if (found)
    {
      return ret;
    }
    // add schema to cache map
    schema *new_schema = schema_alloc(db_home, schema_name, ctx->cf, -1);
    assert(new_schema != NULL);
    ++key_len;
    hashmap_put(ctx->user_map, schema_name, key_len + 1, new_schema, sizeof(void *));
    char wal_path[256] = {'\0'};
    snprintf((char *)&wal_path, 256, "%s/%s/%s", db_home, schema_name, del_kv_wal_log[USER_WAL_LOG_INDEX]);
    new_schema->del_wal_fd = open((char *)&wal_path, O_RDWR | O_CREAT | O_TRUNC);
    schema_put_kv(new_schema, schema_name, key_len + 1, new_schema->meta, new_schema->meta->len);
  }
  return ret;
}
int context_del_schema(context *ctx, char *schema_name)
{
  int ret = -1;
  if (ctx && schema_name)
  {

    size_t key_len = strlen(schema_name);
    hashmap_del(ctx->user_map, schema_name, key_len);

    schema_del_kv(ctx->meta_schema, schema_name, key_len);
    hashmap_del(ctx->sys_map, schema_name, key_len - 1);
    ret = 0;
  }
  return ret;
}

void context_close(context *ctx)
{
  // hashmap_traverse
  if (ctx)
  {
    hashmap_traverse(ctx->sys_map, (hashmap_traverse_cb)&context_schema_traverse);
    hashmap_traverse(ctx->user_map, (hashmap_traverse_cb)&context_schema_traverse);
  }
}