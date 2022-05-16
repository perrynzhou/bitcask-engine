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
static const char *data_file_fmt = "%s/%09d_%d.data";
int data_file_change_read_only(data_file *f)
{
  int ret = -1;
  if (f && f->w_fd != -1)
  {
    char old_filename[256] = {'\0'};
    snprintf((char *)&old_filename, 256, data_file_fmt, f->id, 1);
    char new_filename[256] = {'\0'};
    snprintf((char *)&new_filename, 256, data_file_fmt, f->id, 0);
    rename((char *)&old_filename, (char *)&new_filename);
    close(f->w_fd);
    --f->read_only;
  }
  return ret;
}

data_file *data_file_alloc(char *parent_path, int id, uint64_t max_key_size, uint64_t max_value_size, uint64_t max_file_size)
{
  data_file *f = NULL;

  char filename[256] = {'\0'};
  snprintf((char *)&filename, 256, data_file_fmt, parent_path, id, 1);
  f = (data_file *)calloc(1, sizeof(data_file));
  assert(f != NULL);
  f->w_fd = open((char *)&filename, O_RDWR | O_CREAT | O_APPEND, 0755);
  assert(f->w_fd != -1);
  f->r_fd = open((char *)&filename, O_RDONLY, 0755);
  assert(f->r_fd != -1);
  f->read_only = 0;
  f->max_kv_size = max_key_size + max_value_size;
  f->max_file_size = max_file_size;
  f->cur_size = ATOMIC_VAR_INIT(0);
  f->parent_path = strdup(parent_path);

  return f;
}
ssize_t data_file_write(data_file *f, void *data, size_t len)
{
  ssize_t ret = -1;
  if (f->w_fd != -1 && !f->read_only && len < f->max_kv_size)
  {
    f->cur_size += len;
    ret = write(f->w_fd, data, len);
  }
  return ret;
}
ssize_t data_file_read(data_file *f, int id, size_t offset, void *buf, size_t len)
{
  ssize_t ret = -1;
  if (f->r_fd != -1)
  {

    char filename[256] = {'\0'};
    int flag = (f->id == id) ? 1 : 0;
    snprintf((char *)&filename, 256, data_file_fmt, f->parent_path, id, flag);
    int r_fd = open((char *)&filename, O_RDONLY, 0755);
    lseek(r_fd, offset, SEEK_SET);
    ret = read(r_fd, buf, len);
    close(r_fd);
  }
  return ret;
}
int data_file_destroy(data_file *f)
{

  int ret = -1;
  if (f)
  {
    char filename[256] = {'\0'};
    snprintf((char *)&filename, 256, data_file_fmt, f->id, f->read_only);
    ret = remove((char *)&filename);
  }
  return ret;
}