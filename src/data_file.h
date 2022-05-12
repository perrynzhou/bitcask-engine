/*************************************************************************
    > File Name: data_file.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Thu 12 May 2022 04:30:11 AM UTC
 ************************************************************************/

#ifndef _DATA_FILE_H
#define _DATA_FILE_H
#include "utils.h"
#include "list.h"
const char *data_file_fmt = "%09d_%d.data";
typedef struct data_file
{
  int id;
  int r_fd;
  int w_fd;
  _Atomic(uint64_t) cur_size;
  _Atomic(uint64_t) max_key_size;
  _Atomic(uint64_t) max_value_size;
  _Atomic(uint64_t) max_file_size;
  _Atomic(uint8_t) read_only;
 
  list_node  *cur_node;
} data_file;
data_file *data_file_alloc(int id, uint64_t max_key_size, uint64_t max_value_size, uint64_t max_file_size);
ssize_t data_file_write(data_file *f, void *data, size_t len);
ssize_t data_file_read(data_file *f, size_t offset, char *buf, size_t len);
inline int data_file_change_read_only(data_file *f)
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
int data_file_destroy(data_file *f);
inline void data_file_sync(data_file *f)
{
  if (f && f->w_fd != -1)
  {
    fsync(f->w_fd);
  }
}
#endif
