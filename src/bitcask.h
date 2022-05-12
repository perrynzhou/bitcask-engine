/*************************************************************************
    > File Name: bitcask.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Thu 12 May 2022 02:34:19 AM UTC
 ************************************************************************/

#ifndef _BITCASK_H
#define _BITCASK_H
#include "utils.h"
#include "context.h"
typedef struct bitcask{
  context *ctx;
  conf *cf;
}bitcask;
int bitcask_open(bitcask *bk,const char *conf_file);
int bitcask_create_schema(bitcask *bk,const char *schema_name);
int bitcask_drop_schema(bitcask *bk,const char *schema_name);
int bitcask_dump_schema(bitcask *bk,const char *schema_name);

int bitcask_put(bitcask *bk,const char *schema_name,void *key,size_t key_size,void *value,size_t value_size);
void *bitcask_get(bitcask *bk,const char *schema_name,void *key,size_t key_size);
int bitcask_del(bitcask *bk,const char *schema_name,void *key,size_t key_size);
int bitcask_merge(bitcask *bk,const char *schema_name,void *key,size_t key_size);

void bitcask_close(bitcask *bk);
#endif
