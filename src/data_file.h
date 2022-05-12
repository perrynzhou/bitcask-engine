/*************************************************************************
    > File Name: data_file.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Thu 12 May 2022 04:30:11 AM UTC
 ************************************************************************/

#ifndef _DATA_FILE_H
#define _DATA_FILE_H
#include "utils.h"

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
} data_file;
data_file *data_file_alloc(int id, uint64_t max_key_size, uint64_t max_value_size, uint64_t max_file_size);
ssize_t data_file_write(data_file *f, void *data, size_t len);
ssize_t data_file_read(data_file *f, size_t offset, void *buf, size_t len);
int data_file_change_read_only(data_file *f);
int data_file_destroy(data_file *f);
inline void data_file_sync(data_file *f)
{
  if (f && f->w_fd != -1)
  {
    fsync(f->w_fd);
  }
}
#endif
