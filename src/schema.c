/*************************************************************************
    > File Name: schema.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Sat 07 May 2022 06:50:53 AM UTC
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "schema.h"
#include "kv_value.h"
#include "kv_index.h"
#include "log.h"
#include "utils.h"
#include "schema_meta.h"
#include "meta.h"
#include "art.h"
#include "crc.h"
#include "art.h"
#define SCHEMA_DATA_FILE_MIN_LEN (64)

int schema_add_data_file(schema *m, int fid)
{
  int ret = -1;
  if (m && fid >= 0)
  {
    if (fid <= 0)
    {
      m->files = (data_file **)calloc(1, sizeof(data_file *) * SCHEMA_DATA_FILE_MIN_LEN);
    }
    if (fid > SCHEMA_DATA_FILE_MIN_LEN)
    {
      m->files = (data_file **)realloc(m->files, SCHEMA_DATA_FILE_MIN_LEN * 2);
    }
    //  schema_data_home
    conf *cf = m->cf;
    data_file *cur_data_file = data_file_alloc(m->data_home, fid, cf);
    m->files[m->fid] = cur_data_file;
    atomic_fetch_add(&m->meta->file_cnt, 1);
    atomic_fetch_add(&m->fid, 1);
    ret = 0;
  }
  return ret;
}

int load_schema_data_from_file(schema *sch, void *ctx1, void *ctx2, schema_load_cb cb)
{
  int ret = -1;
  if(sch !=NULL) {
     for(uint32_t i=0;i<sch->fid;i++) {
       char fd_path[256] = {'\0'};
       char *fd_path_ptr = (char *)&fd_path;
       snprintf(fd_path_ptr,256,"%s/%09d.data",sch->data_home,i);
       struct stat st;
       if(stat(fd_path_ptr,&st)!=0){
         slog_warn("%s ,fd = %s not exists",sch->meta->name,fd_path_ptr);
          continue;
       }
       bool is_first = false;
       int fd = open(fd_path_ptr,O_RDONLY);
       assert(fd !=-1);
       size_t fd_sz = st.st_size;
       conf *cf = (conf *)ctx2;
       size_t kv_max_size = cf->max_key_size+cf->max_value_size;
       char  *kv_max_buffer = (char *)calloc(1,kv_max_size);
       char   *tmp_key_buffer = (char *)calloc(1,cf->max_key_size+1);
       while(fd_sz>0) {
          kv_value tmp;
          int f_rn = read(fd,&tmp,sizeof(tmp));
          if(f_rn <=0) {
            break;
          }
          int s_rn = read(fd,kv_max_buffer,kv_max_size);
          if(s_rn <=0) {
            break;
          }
          size_t rn = f_rn+s_rn+sizeof(tmp);
          size_t offset = 0;
          if(!is_first) {
            is_first = true;
          }else {
             offset = rn;
          }
          kv_index *idx = kv_index_alloc(i,offset,rn);
          memcpy(tmp_key_buffer,kv_max_buffer,tmp.k_sz);
          art_insert(&sch->index_tree,(const unsigned char *)tmp_key_buffer,tmp.k_sz,idx);
          memset(kv_max_buffer,0,kv_max_size);
          
          fd_sz = fd_sz - rn;
       }
       if(kv_max_buffer !=NULL)
       {
         free(kv_max_buffer);
       }
       if(fd !=-1) 
       {
         close(fd);
       }
     }
  }
  return ret;
}
int schema_drop_data_file(schema *m, int index)
{
  int ret = -1;
  if (m && index >= 0)
  {
    struct stat st;
    char data_file_path[256] = {'\0'};
    snprintf((char *)&data_file_path, 256, "%s/%s/%09d.data", m->data_home, m->meta->name, index);
    ret = stat((char *)&data_file_path, &st);
    if (!ret)
    {
      ret = remove((char *)&data_file_path);
    }
  }
  return 0;
}

schema *schema_alloc_from_meta(schema_meta *m, conf *cf)
{
  schema *s = NULL;
  if (m)
  {
    s = schema_alloc(cf->db_home, m->name, cf);

    for (size_t i = 0; i < m->file_cnt; i++)
    {
      schema_add_data_file(s, i);
    }
  }
  return s;
}
schema *load_schema_meta_from_file(const char *path, void *ctx1,void *ctx2, schema_load_cb cb)
{
  schema *res = NULL;
  if (path)
  {
    struct stat st;
    stat(path, &st);

    conf *cf = (conf *)ctx2;
    res=schema_alloc(cf->db_home,sys_schema[SYS_SCHEMA_META_INDEX],cf);
    assert(res != NULL);

    schema_add_data_file(res,0);
    int fd = open(path, O_RDONLY);
    assert(fd != -1);
    int i = 0;
    size_t fsz = st.st_size;
    size_t rn = sizeof(kv_value)+sizeof(schema_meta);
    kv_value *val = (kv_value *)calloc(1,rn);
    while (fsz > 0)
    {
      
      int ret = read(fd, val, rn);
      if (ret <= 0)
      {
        break;
      }
      schema_meta *tmp = (schema_meta *)val->kv;
      if (tmp->active)
      {
      
        size_t  offset = i *rn;
        kv_index *idx = kv_index_alloc(0, offset, sizeof(*tmp));
        char *name = (char *)tmp->name;
        art_insert(&res->index_tree, (const unsigned char *)name, strlen(name), idx);
      
      if (ctx1 && ctx2 && cb)
      {
        cb(ctx1, ctx2, tmp);
      }
      }

      fsz -= rn;
      i++;
    }
    if(fd !=-1) {
      close(fd);
    }
    if(val !=NULL) 
    {
      free(val);
    }
  }
  return res;
}
schema *schema_alloc(const char *db_home, const char *name, conf *cf)
{
  schema *s = NULL;
  if (db_home && name)
  {

    s = (schema *)calloc(1, sizeof(schema));
    assert(s != NULL);
    s->meta = schema_meta_alloc();
    schema_meta_init(s->meta, 0, 0, 1, 1, name);
    assert(s->meta != NULL);
    char schema_path[256] = {'\0'};
    snprintf((char *)&schema_path, 256, "%s/%s", db_home, name);
    s->data_home = strdup((char *)&schema_path);

    if (access((char *)&schema_path, F_OK) != 0)
    {
      mkdir((char *)&schema_path, 0755);
    }
    art_tree_init(&s->index_tree);
    s->cf = cf;
    s->fid = 0;
  }
  return s;
}
void schema_close(schema *s)
{
  if (s)
  {
    for (size_t i = 0; i < s->meta->file_cnt; i++)
    {

      data_file *d = s->files[i];
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
inline static int schema_drop(schema *s)
{
  for (size_t i = 0; i < s->meta->file_cnt; i++)
  {
    char buf[256] = {'\0'};
    snprintf((char *)&buf, 256, "%s/%09ld.data", s->data_home, i);
    slog_info("remove %s,data_file=%s", s->meta->name, (char *)&buf);
    remove((char *)&buf);
  }
  return 0;
}
void schema_destroy(schema *sch)
{

  schema_drop(sch);
  schema_close(sch);
}

int schema_put_kv(schema *sch, void *key, size_t key_sz, void *value, size_t value_sz)
{
  if (sch->files[sch->meta->file_cnt - 1]->cur_size >= sch->cf->max_data_file_size)
  {
    pthread_mutex_lock(&sch->lock);
    schema_add_data_file(sch, sch->fid);
    pthread_mutex_unlock(&sch->lock);
  }

  size_t cur_file_index = sch->fid - 1;
  data_file *cur_file = sch->files[cur_file_index];
   kv_value *val = NULL;
  if(strcmp((char *)&sch->meta->name,sys_schema[SYS_SCHEMA_META_INDEX])!=0) {
     val = kv_value_alloc(key, key_sz, value, value_sz);
  }else {
    // just save schema_meta info on disk
     val = kv_value_alloc(NULL, 0, value, value_sz);
  }
  size_t write_sz = sizeof(*val) + val->k_sz + val->v_sz;
  size_t offset = sch->files[cur_file_index]->cur_size;

  if (data_file_write(sch->files[cur_file_index], val, write_sz) > 0)
  {
    slog_info("offset=%ld,write_sz=%ld", offset, write_sz);
    void *found = art_search(&sch->index_tree, (const unsigned char *)key, key_sz);
    if (!found)
    {
      kv_index *idx = kv_index_alloc(cur_file->fid, offset, write_sz);
      art_insert(&sch->index_tree, (const unsigned char *)key, key_sz, idx);
      atomic_fetch_add(&sch->meta->kv_count, 1);
    }
    else
    {
      kv_index tmp_idx;
      kv_index_init(&tmp_idx, cur_file->fid, offset, write_sz);
      kv_index *idx = (kv_index *)found;
      memcpy(idx, &tmp_idx, sizeof(kv_index));
    }
  }
  return 0;
}
void *schema_get_kv(schema *sch, void *key, size_t key_sz)
{
  void *value_ptr = NULL;
  if (sch && key)
  {
    void *found = art_search(&sch->index_tree, (const unsigned char *)key, key_sz);
    if (found)
    {
      kv_index *idx = (kv_index *)found;
      data_file *cur_file = sch->files[idx->fid];
      kv_value *value = (kv_value *)calloc(1, idx->size);
      data_file_read(cur_file, idx->offset, value, idx->size);
      uint32_t crc = crc32(value->kv, value->k_sz + value->v_sz);
      slog_info("source crc:%d,now crc:%d\n", value->crc, crc);
      value_ptr = (char *)value->kv + value->k_sz;
    }
  }
  return value_ptr;
}

int schema_del_kv(schema *sch, void *key, size_t key_sz)
{
  int ret = -1;
  if (sch && key)
  {
    void *found = art_search(&sch->index_tree, (const unsigned char *)key, key_sz);
    if (found)
    {
      art_delete(&sch->index_tree, key, key_sz);
      ret = 0;
    }
  }
  return ret;
}
