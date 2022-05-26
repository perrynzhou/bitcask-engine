/*************************************************************************
    > File Name: data_file.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Thu 12 May 2022 04:32:07 AM UTC
 ************************************************************************/

#include <stdio.h>
#include <stdatomic.h>
#include "data_file.h"
#include "log.h"
#include "conf.h"

data_file *data_file_alloc(char *parent_path, int fid, conf *cf)
{
  data_file *f = NULL;

  char filename[256] = {'\0'};
  snprintf((char *)&filename, 256, "%s/%09d.data", parent_path, fid);
  f = (data_file *)calloc(1, sizeof(data_file));
  assert(f != NULL);
  f->w_fd = open((char *)&filename, O_RDWR | O_CREAT | O_APPEND, 0755);
  assert(f->w_fd != -1);
  f->r_fd = open((char *)&filename, O_RDONLY, 0755);
  assert(f->r_fd != -1);
  f->fid=fid;
  f->max_kv_size = cf->max_key_size + cf->max_value_size;
  f->max_file_size = cf->max_data_file_size;
  f->cur_size = ATOMIC_VAR_INIT(0);
  f->parent_path = strdup(parent_path);
  return f;
}
ssize_t data_file_write(data_file *f, void *data, size_t len)
{
  ssize_t ret = -1;
  if (f->w_fd != -1 && len < f->max_kv_size)
  {
    
    atomic_fetch_add(&f->cur_size,len);
    ret = write(f->w_fd, data, len);
    fsync(f->w_fd);
  }
  return ret;
}
ssize_t data_file_read(data_file *f,  size_t offset, void *buf, size_t len)
{
  ssize_t ret = -1;
  if (f->r_fd != -1)
  {
    lseek(f->r_fd, offset, SEEK_SET);
    ret = read(f->r_fd, buf, len);
  }
  return ret;
}
int data_file_destroy(data_file *f)
{
  int ret = -1;
  if (f)
  {
    char filename[256] = {'\0'};
    snprintf((char *)&filename, 256, f->parent_path, f->fid);
    ret = remove((char *)&filename);
  }
  return ret;
}