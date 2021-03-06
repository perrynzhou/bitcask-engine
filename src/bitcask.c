/*************************************************************************
    > File Name: bitcask.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Thu 12 May 2022 02:34:25 AM UTC
 ************************************************************************/

#include <stdio.h>
#include "bitcask.h"
#include "log.h"
int bitcask_open(bitcask *bk,const char *conf_file)
{
  int ret = -1;
  if(bk && conf_file) {
    conf *cf = conf_create(conf_file);
    assert(cf !=NULL);
    bk->cf = cf;
    bk->ctx = context_open(cf);
    assert(bk->ctx !=NULL);
    ret = 0;
  }
  return ret;

}
int bitcask_create_schema(bitcask *bk,const char *schema_name)
{
  int ret = -1;
  if(bk && schema_name) {
    ret = context_put_schema(bk->ctx,(char *)schema_name);
  }
  return ret;
}
void *bitcask_fetch_schema(bitcask *bk,const char *schema_name){
   void *m = NULL;
   if(bk && schema_name) {
     m = context_get_schema(bk->ctx,(char *)schema_name);
     if(m) {
        schema *tmp = (schema *)m;
       // slog_info("name=%s,file_cnt=%d,active=%d",tmp->meta->name,tmp->meta->data_file_cnt,tmp->meta->active);
     }
   }
   return m;
   
}
int bitcask_drop_schema(bitcask *bk,const char *schema_name)
{
  int ret = -1;
  if(bk && schema_name) {
    ret = context_del_schema(bk->ctx,(char *)schema_name);
  }
  return ret;
}
int bitcask_dump_schema(bitcask *bk,const char *schema_name)
{
  //todo
  return -1;
}

int bitcask_put(bitcask *bk,const char *schema_name,void *key,size_t key_size,void *value,size_t value_size)
{
  int ret = -1;
  if(bk && schema_name && key && value) {
     schema *s = (schema *)bitcask_fetch_schema(bk,schema_name);
     if(!s) {
       return ret;
     }
     ret = schema_put_kv(s,key,key_size,value,value_size);
  }
  return ret;
}
void *bitcask_get(bitcask *bk,const char *schema_name,void *key,size_t key_size)
{
 void *value_ptr = NULL;
  if(bk && schema_name && key) {
     schema *s = (schema *)bitcask_fetch_schema(bk,schema_name);
     if(s) {
       value_ptr =  schema_get_kv(s,key,key_size);
     }
  }
  return value_ptr;

}
int bitcask_del(bitcask *bk,const char *schema_name,void *key,size_t key_size)
{
  int ret = -1;
  if(bk && schema_name && key) {
     schema *s = (schema *)hashmap_get(bk->ctx->schema_cache,(void *)schema_name,strlen(schema_name));
     if(!s) {
       return ret;
     }
     ret = schema_del_kv(s,key,key_size);
  }
  return ret;

}
int bitcask_merge(bitcask *bk,const char *schema_name,void *key,size_t key_size)
{
 //todo
 return 0;
}
void bitcask_close(bitcask *bk) {
  
}