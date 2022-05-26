/*************************************************************************
    > File Name: schema_meta.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Sat 14 May 2022 10:18:51 AM UTC
 ************************************************************************/

#include <stdio.h>

#include <stdatomic.h>
#include "schema_meta.h"
int schema_meta_init(schema_meta *m,uint64_t bytes,uint64_t count,uint8_t active,uint32_t file_cnt,const char *name)
{
  int ret = -1;
  if(m) {
    size_t name_len = strlen(name);
    m = (schema_meta *)calloc(1, sizeof(schema_meta));
    assert(m != NULL);
    m->kv_count = bytes;
    m->file_cnt = count;
    m->active = active;
    strncpy((char *)&m->name, name, name_len);
  }
  return ret;
}
schema_meta *schema_meta_alloc()
{
  schema_meta *m = (schema_meta *)calloc(1,sizeof(schema_meta));
  return m;
}
int   schema_meta_save(schema_meta *meta,int fd)
{
  int ret =-1;
  if(meta && fd !=-1) {
      ret = write(fd,meta,sizeof(*meta));
  }
  return ret;
}
void schema_meta_destroy(schema_meta *sm)
{
  if (sm)
  {
    free(sm);
    sm = NULL;
  }
}