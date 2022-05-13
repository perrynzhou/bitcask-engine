/*************************************************************************
    > File Name: context.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Sat 07 May 2022 06:49:58 AM UTC
 ************************************************************************/

#ifndef _CONTEXT_H
#define _CONTEXT_H
#include "hashmap.h"
#include "schema.h"
#include "conf.h"
#define SYS_WAL_LOG_INDEX 0
#define USER_WAL_LOG_INDEX 1

typedef struct context {
  conf  *cf;
  hashmap *user_map;
  hashmap  *sys_map;
  schema   *meta_schema;
  int      sys_wal_log_fd;
}context;
context *context_open(conf *cf);
int context_put_schema(context *ctx, char *schema_name);
int context_del_schema(context *ctx, char *schema_name);
void *context_get_schema(context *ctx, char *schema_name,bool flag);
void context_close(context *ctx);
#endif
