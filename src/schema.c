/*************************************************************************
    > File Name: schema.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Sat 07 May 2022 06:50:53 AM UTC
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include "schema.h"
#define SCHEMA_BLOCK_SIZE (1024 * 1024)
schema *schema_alloc(const char *name)
{
  schema *s = NULL;
  if (name)
  {

    s = (schema *)calloc(1, sizeof(schema));
    assert(s != NULL);
    size_t name_len = strlen(name)+1;
    schema_meta *meta = (schema_meta *)calloc(1,sizeof(schema_meta)+name_len);
    assert(meta !=NULL);
    meta->is_active=1;
    strncpy((char *)&meta->name,name,name_len);
    meta->bytes = ATOMIC_VAR_INIT(0);
    meta->obj_count = ATOMIC_VAR_INIT(0);
    // m->fd = open(name,O_RDWR|O_CREAT|O_APPEND);
    s->fd = open(name, O_RDWR | O_CREAT);

    assert(s->fd != -1);
    s->data = bplus_tree_init(meta->name, s->fd, SCHEMA_BLOCK_SIZE);
    s->meta = meta;
  }
  return s;
}
void schema_destroy(schema *s, bool is_drop)
{
  if (s)
  {
    close(s->fd);
    bplus_tree_deinit(s->data);
    if (is_drop)
    {
      remove((char *)&s->meta->name);
    }
    free(s->meta);
    free(s);
    s = NULL;
  }
}