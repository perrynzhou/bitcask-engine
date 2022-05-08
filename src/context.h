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
typedef struct context {
  char *home;
  hashmap *user_map;
  hashmap  *sys_map;
  schema   *meta_schema;
}context;
context *context_open(const char *home);
int context_put_schema(context *ctx, char *schema_name);
int context_del_schema(context *ctx, char *schema_name);
void context_close(context *ctx);
#endif
