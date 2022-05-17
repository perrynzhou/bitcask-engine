/*************************************************************************
    > File Name: schema_meta.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Sat 14 May 2022 10:18:51 AM UTC
 ************************************************************************/

#include <stdio.h>

#include <stdatomic.h>
#include "schema_meta.h"
schema_meta *schema_meta_alloc(const char *name)
{
  schema_meta *smeta = NULL;
  if (name)
  {
    size_t name_len = strlen(name);
    size_t meta_size = sizeof(schema_meta) + name_len+1;
    smeta = (schema_meta *)calloc(1, meta_size);
    assert(smeta != NULL);
    smeta->kv_count = ATOMIC_VAR_INIT(0);
    smeta->data_file_cnt = ATOMIC_VAR_INIT(0);
    smeta->len = meta_size;
    smeta->active = 1;
    strncpy((char *)&smeta->name, name, name_len);
  }
  return smeta;
}

void schema_meta_destroy(schema_meta *sm)
{
  if (sm)
  {
    free(sm);
    sm = NULL;
  }
}