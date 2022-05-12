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
#define SCHEMA_DATA_FILE_MIN_LEN (64)
schema *schema_alloc(const char *db_home, const char *name, conf *cf,int del_wal_fd)
{
  schema *s = NULL;
  if (name)
  {

    s = (schema *)calloc(1, sizeof(schema));
    assert(s != NULL);
    size_t name_len = strlen(name) + 1;
    schema_meta *meta = (schema_meta *)calloc(1, sizeof(schema_meta) + name_len+1);
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
    meta->kv_count = ATOMIC_VAR_INIT(0);
    meta->data_file_cnt = ATOMIC_VAR_INIT(0);
    meta->len = sizeof(schema_meta) + name_len+1;
    art_tree_init(&s->index_tree);
    s->cf = cf;
    s->del_wal_fd =del_wal_fd;

    data_file *cur_data_file = data_file_alloc(0, cf->max_key_size, cf->max_value_size, cf->max_data_file_size);
    s->files = (data_file **)calloc(1, sizeof(data_file *) * SCHEMA_DATA_FILE_MIN_LEN);
    s->files[s->data_file_id] = cur_data_file;
    ++s->meta->data_file_cnt;
  }
  return s;
}
void schema_close(schema *m)
{
  if (m)
  {
    for (size_t i = 0; i < m->meta->data_file_cnt; i++)
    {

      data_file *d = m->files[i];
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

int schema_put_kv(schema *m, void *key, size_t key_sz, void *value, size_t value_sz)
{
  if (m->files[m->meta->data_file_cnt - 1]->cur_size >= m->cf->max_data_file_size)
  {
    pthread_mutex_lock(&m->lock);
    data_file *new_file = data_file_alloc(m->data_file_id, m->cf->max_key_size, m->cf->max_value_size, m->cf->max_data_file_size);
    m->files[m->data_file_id] = new_file;
    ++m->data_file_id;
    pthread_mutex_unlock(&m->lock);
  }
  size_t file_index = m->data_file_id - 1;
  entry *et = entry_alloc(key, key_sz, value, value_sz);
  size_t write_sz = sizeof(*et) + et->k_sz + et->v_sz;
  data_file_write(m->files[file_index], et, write_sz);

  struct stat s;
  fstat(m->files[file_index]->w_fd, &s);
  size_t offset =s.st_size;

  item *itm = item_alloc(m->files[file_index]->id, offset, write_sz);
  void *found = art_search(&m->index_tree,(const unsigned char *) key, key_sz);
  if (!found)
  {
    art_insert(&m->index_tree, (const unsigned char *)key, key_sz, itm);
    ++m->meta->kv_count;
  }else {
      // save invalid k/v
      item *it = (item *)found;
      write(m->del_wal_fd,(void *)it,sizeof(item));
      fsync(m->del_wal_fd);
      // update mem
      memcpy(found,itm,sizeof(item));
  }
  return 0;
}
void *schema_get_kv(schema *m, void *key, size_t key_sz)
{
  void *value_ptr = NULL;
  if (m && key)
  {
    void *found = art_search(&m->index_tree, (const unsigned char *)key, key_sz);
    if (found)
    {
      item *it = (item *)found;
      data_file *cur_file = m->files[it->fid];
      value_ptr = calloc(1, it->size);
      data_file_read(cur_file, it->offset, value_ptr, it->size);
    }
  }
  return value_ptr;
}

int schema_del_kv(schema *m, void *key, size_t key_sz) {
  int ret = -1;
  if (m && key)
  {
    void *found = art_search(&m->index_tree, (const unsigned char *)key, key_sz);
    if (found)
    {
      item *it = (item *)found;
      write(m->del_wal_fd,(void *)it,sizeof(item));
      fsync(m->del_wal_fd);
      ret = 0;
    }
  }
  return ret;
}