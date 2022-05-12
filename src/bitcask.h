/*************************************************************************
    > File Name: bitcask.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Thu 12 May 2022 02:34:19 AM UTC
 ************************************************************************/

#ifndef _BITCASK_H
#define _BITCASK_H
#include "utils.h"
typedef struct bitcask{

}bitcask;
int bitcask_open(bitcask *bk);
int bitcask_put(bitcask *bk,void *key,size_t key_size,void *value,size_t value_size);
void *bitcask_get(bitcask *bk,void *key,size_t key_size,size_t *value_size);
int bitcask_delete(bitcask *bk,void *key,size_t key_size);
int bitcask_merge(bitcask *bk,void *key,size_t key_size);

void bitcask_close(bitcask *bk);
#endif
