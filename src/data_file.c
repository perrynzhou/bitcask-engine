/*************************************************************************
    > File Name: data_file.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Thu 12 May 2022 04:32:07 AM UTC
 ************************************************************************/

#include <stdio.h>
#include <stdatomic.h>
#include "data_file.h"

data_file *data_file_alloc(int id, uint64_t max_key_size,uint64_t max_value_size,uint64_t  max_file_size)
{
  data_file *f = NULL;
  
    char filename[256] = {'\0'};
    snprintf((char *)&filename,256,data_file_fmt,id,1);
    f = (data_file *)calloc(1,sizeof(data_file));
    assert(f !=NULL);
    f->w_fd = open((char *)&filename,O_RDWR|O_CREAT|O_APPEND);
    assert(f->w_fd !=-1);
    f->r_fd = open((char *)&filename,O_RDONLY);
    assert(f->r_fd !=-1);
    f->read_only=0;
    f->max_key_size=max_key_size;
    f->max_file_size=max_file_size;
    f->max_value_size=max_value_size;
    f->cur_node = list_alloc();
    f->cur_size =ATOMIC_VAR_INIT(0);
    assert(f->cur_node !=NULL);
    f->cur_node->next = f->cur_node->prev = NULL;
  return f;

}
ssize_t  data_file_write(data_file *f,void *data,size_t len)
{
  if(f->w_fd !=-1) {
       f->cur_size += len;

      return  write(f->w_fd,data,len);
      
  }
  return -1;
  
}
ssize_t  data_file_read(data_file *f,size_t offset,char *buf,size_t len)
{
  if(f->r_fd !=-1) {
  lseek(f->r_fd,offset,SEEK_SET);
   return read(f->r_fd,buf,len);
  }
  return -1;
  
}
int  data_file_destroy(data_file *f)
{

  int ret = -1;
   if(f) {
    char filename[256] = {'\0'};
    snprintf((char *)&filename,256,data_file_fmt,f->id,f->read_only);
    ret =remove((char *)&filename);
   }
   return ret;
}