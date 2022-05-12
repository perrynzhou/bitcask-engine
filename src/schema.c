/*************************************************************************
    > File Name: schema.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Sat 07 May 2022 06:50:53 AM UTC
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include "schema.h"
#include "entry.h"
#include "item.h"
#define SCHEMA_BLOCK_SIZE (1024 * 1024)
schema *schema_alloc(const char *db_home, const char *name, conf *cf)
{
  schema *s = NULL;
  if (name)
  {

    s = (schema *)calloc(1, sizeof(schema));
    assert(s != NULL);
    size_t name_len = strlen(name) + 1;
    schema_meta *meta = (schema_meta *)calloc(1, sizeof(schema_meta) + name_len);
    assert(meta != NULL);
    meta->active = 1;
    strncpy((char *)&meta->name, name, name_len);
    s->db_home = strdup(db_home);
    s->data_file_id = ATOMIC_VAR_INIT(0);
    char schema_path[256] = {'\0'};
    snprintf((char *)&schema_path, 256, "%s/%s", db_home, name);
    if (access((char *)&schema_path, F_OK) != 0)
    {
      mkdir((char *)&schema_path, 0755);
    }
    meta->bytes = ATOMIC_VAR_INIT(0);
    meta->kv_count = ATOMIC_VAR_INIT(0);
    art_tree_init(&s->index_tree);
    s->cf=cf;
    data_file *cur_data_file = data_file_alloc(0, cf->max_key_size, cf->max_value_size, cf->max_data_file_size);
    s->cur_file = cur_data_file;
  }
  return s;
}
void schema_close(schema *m)
{
  if (m)
  {
    list_node *head = m->cur_file->cur_node;
    while (head != NULL)
    {
      data_file *d = container_of(head, data_file, id);
      if (d->r_fd != -1)
      {
        close(d->r_fd);
      }
      if (d->w_fd != -1)
      {
        close(d->w_fd);
      }
    }
  }
}
void schema_destroy(schema *m)
{
  schema_close(m);
  char schema_path[256] = {'\0'};
  snprintf((char *)&schema_path, 256, "%s/%s", m->db_home, (char *)&m->meta->name);
  remove((char *)&schema_path);
}

int schema_put(schema *m,void *key,size_t key_sz,void *value,size_t value_sz)
{
  if(m->cur_file->cur_size >= m->cf->max_data_file_size) {
     pthread_mutex_lock(&m->lock);
      data_file *new_file = data_file_alloc(++m->data_file_id, m->cf->max_key_size, m->cf->max_value_size, m->cf->max_data_file_size);
      list_add(new_file->cur_node,m->cur_file->cur_node);
      m->cur_file = new_file;
     pthread_mutex_unlock(&m->lock);
  }
  entry  *et = entry_alloc(key,key_sz,value,value_sz);
  size_t write_sz = sizeof(*et)+et->k_sz+et->v_sz;
  data_file_write(m->cur_file,et,write_sz);
  size_t offset = lseek(m->cur_file->w_fd, 0, SEEK_CUR);
  item *itm = item_alloc(m->cur_file->id,offset,write_sz);
  char *key_ptr = (char *)key;
   size_t key_len = strlen(key_ptr);
   void *found = art_search(&m->index_tree,key_ptr,key_len);
   if(!found) {
     art_insert(&m->index_tree,key_ptr,key_len,itm);
   }
}