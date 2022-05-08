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
#include "object.h"
#include "hashmap.h"
#include "hash.h"
#include "log.h"
#define SCHEMA_BUF_SIZE (496)
#define CONTEXT_SYSSCHEMA_INSIDE_COUNT (64)
#define CONTEXT_USERSCHEMA_INSIDE_COUNT (65535)

static int *context_schema_traverse(void *k, void *v)
{
  char *schema_name = (char *)k;
  schema *m = (schema *)v;
  close(m->fd);
  schema_destroy(m,false);
  logi("close %s ", schema_name);
  return 0;
}
context *context_open(const char *home)
{
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
  ctx->home = strdup(home);
  ctx->sys_map = hashmap_alloc(CONTEXT_SYSSCHEMA_INSIDE_COUNT, (hashmap_hash_cb)&hash_fnv1_32, (hashmap_key_compare_cb)&memcmp);
  ctx->user_map = hashmap_alloc(CONTEXT_USERSCHEMA_INSIDE_COUNT, (hashmap_hash_cb)&hash_fnv1_32, (hashmap_key_compare_cb)&memcmp);

  size_t sys_schema_count = sizeof(sys_schema) / sizeof(sys_schema[0]);
  schema *meta_schema = NULL;
  for (size_t i = 0; i < sys_schema_count; i++)
  {
    char buf[SCHEMA_BUF_SIZE] = {'\0'};
    snprintf((char *)&buf, SCHEMA_BUF_SIZE, "%s/%s.bt", home, sys_schema[i]);
    size_t buf_len = strlen((char *)&buf);
    schema *sm = schema_alloc((char *)&buf);
    if(strncmp((char *)&buf,"meta",4)==0) {
      meta_schema = sm;
    }
    int ret = hashmap_put(ctx->sys_map, (char *)&buf, buf_len, sm, sizeof(schema *));
    logi("load %s schema,ret=%d", (char *)&buf, ret);
    ctx->meta_schema = meta_schema;
  }
  return ctx;
}
int context_put_schema(context *ctx, char *schema_name)
{
  int ret = -1;
  if(ctx && schema_name) {
      // add schema to cache map
      schema *new_schema = schema_alloc(schema_name);
      assert(new_schema !=NULL);
      size_t key_len = strlen(schema_name)+1;
      hashmap_put(ctx->user_map,schema_name,key_len,new_schema,sizeof(void *));
   

      // add schema rec  to meta schema
      uint32_t hash_code = hash_fnv1_32(schema_name,key_len-1);
      object *value = (object *)calloc(1,sizeof(object)+sizeof(new_schema->meta));
      memcpy(value->data,new_schema->meta,sizeof(schema_meta));
      bplus_tree_put(ctx->meta_schema->data,hash_code,(long)value);
  }
  return ret;
}
int context_del_schema(context *ctx, char *schema_name)
{
  int ret = -1;
  if(ctx && schema_name) {

      size_t key_len = strlen(schema_name)+1;
      hashmap_del(ctx->user_map,schema_name,key_len);

      uint32_t hash_code = hash_fnv1_32(schema_name,key_len-1);
      bplus_tree_del(ctx->meta_schema->data,hash_code);
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