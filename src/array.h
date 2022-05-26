/*************************************************************************
    > File Name: array.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Mon 28 Mar 2022 05:37:57 AM UTC
 ************************************************************************/


#ifndef _ARRAY_H
#define _ARRAY_H
#include <stdint.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
typedef int (*array_traverse_cb)(void *);
typedef struct array
{
  void **ptr;
  _Atomic(size_t) cur_count;
  _Atomic(size_t) max_count;
  bool auto_expand;
} array;

array *array_alloc(size_t max_count, bool auto_expand);
int array_init(array *sk, size_t max_count, bool auto_expand);
int array_add(array *sk, void *data,size_t index);
int array_push_back(array *sk,void *data);
void *array_del(array *sk,size_t index);
void array_deinit(array *sk);
void array_destroy(array *sk);
int array_traverse(array *sk, array_traverse_cb traverse_cb);
void *array_value(array *sk,size_t index);

inline static size_t array_len(array *sk)
{
  if (sk != NULL)
  {
    return sk->cur_count;
  }
  return -1;
}
inline static bool array_empty(array *sk)
{
  if (sk != NULL && sk->cur_count > 0)
  {
    return true;
  }
  return false;
}
#endif


