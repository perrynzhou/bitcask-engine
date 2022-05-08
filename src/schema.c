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
  schema *m = NULL;
  if (name)
  {

    m = (schema *)calloc(1, sizeof(schema));
    assert(m != NULL);
    m->bytes = ATOMIC_VAR_INIT(0);
    m->obj_count = ATOMIC_VAR_INIT(0);
    // m->fd = open(name,O_RDWR|O_CREAT|O_APPEND);
    m->fd = open(name, O_RDWR | O_CREAT);

    assert(m->fd != -1);
    m->name = strdup(name);
    m->data = bplus_tree_init(m->name, m->fd, SCHEMA_BLOCK_SIZE);
  }
  return m;
}
void schema_destroy(schema *m, bool is_drop)
{
  if (m)
  {
    close(m->fd);
    bplus_tree_deinit(m->data);
    if (is_drop)
    {
      remove(m->name);
    }
    free(m->name);
    free(m);
    m = NULL;
  }
}