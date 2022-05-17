/*************************************************************************
    > File Name: array.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Mon 28 Mar 2022 05:37:42 AM UTC
 ************************************************************************/

#include "array.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#define ARRAY_INCR_FACCTOR (0.25)
#define ARRAY_INIT_SIZE (8)
array *array_alloc(size_t max_count, bool auto_expand)
{
  array *sk = (array *)calloc(1, sizeof(array));
  assert(sk != NULL);
  if (array_init(sk, max_count, auto_expand) != 0)
  {
    free(sk);
    sk = NULL;
  }
  return sk;
}
static inline int array_expand(array *sk)
{
  int ret = -1;
  if (sk)
  {
    size_t max_count = sk->max_count * (1 + ARRAY_INCR_FACCTOR);
    sk->ptr = (void **)realloc(sk->ptr, sizeof(void *) * max_count);
    sk->max_count = max_count;
    ret = 0;
  }
  return ret;
}
int array_init(array *sk, size_t max_count, bool auto_expand)
{
  if (sk != NULL && max_count > 0)
  {
    max_count = (max_count <= ARRAY_INIT_SIZE) ? ARRAY_INIT_SIZE : max_count;
    sk->ptr = (void **)calloc(max_count, sizeof(void *));
    assert(sk->ptr != NULL);
    sk->cur_count = ATOMIC_VAR_INIT(0);
    sk->max_count = max_count;
    sk->auto_expand = auto_expand;
    return 0;
  }
  return -1;
}
int array_add(array *sk, void *data, size_t index)
{
  if (sk != NULL && data != NULL && index < sk->max_count)
  {
    if (sk->cur_count >= sk->max_count && sk->auto_expand && array_expand(sk) != 0)
    {
      return -1;
    }
    bool flag = (index >= sk->cur_count) ? true : false;
    if (!flag)
    {
      for (size_t i = sk->cur_count; i >= index; i--)
      {
        sk->ptr[i] = sk->ptr[i - 1];
      }
      sk->ptr[index] = data;
    }
    else
    {
      sk->ptr[sk->cur_count] = data;
    }
    atomic_fetch_add(&sk->cur_count, 1);
    return 0;
  }
  return -1;
}

void *array_del(array *sk, size_t index)
{
  void *ptr = NULL;
  if (sk && index < sk->max_count)
  {
    ptr = sk->ptr[index];
    for (size_t i = index; i < sk->max_count; i++)
    {
      sk->ptr[index] = sk->ptr[index + 1];
    }
    atomic_fetch_sub(&sk->cur_count, 1);
    if(sk->ptr[sk->max_count-1]) {
      sk->ptr[sk->max_count-1] = NULL;
    }
  }
  return ptr;
}
void array_deinit(array *sk)
{
  if (sk != NULL && sk->ptr != NULL)
  {
    free(sk->ptr);
    sk->ptr = NULL;
  }
}
void array_destroy(array *sk)
{
  if (sk != NULL)
  {
    array_deinit(sk);
    free(sk);
    sk = NULL;
  }
}
int array_traverse(array *sk, array_traverse_cb traverse_cb)
{
  if (sk != NULL && sk->ptr != NULL)
  {
    for (size_t i = 0; i < sk->cur_count; i++)
    {
      if (traverse_cb != NULL)
      {
        traverse_cb(sk->ptr[i]);
      }
    }
  }
  return 0;
}
#ifdef TEST
#include <threads.h>
#include <stdatomic.h>
#include <stdlib.h>
int test_array_traverse_cb(void *arg)
{
  char *v = (char *)arg;
  fprintf(stdout, "traverse:value=%s\n", v);
  return 0;
}

int main(int argc, char *argv[])
{
  int n = atoi(argv[1]);
  array sk;
  array_init(&sk, 32, true);
  for (int i = 0; i < n; i++)
  {
    char buf[256] = {'\0'};
    snprintf(&buf, 256, "data-%d", i);
    char *data = strdup((char *)&buf);
    int ret = array_add(&sk, data, i);
    fprintf(stdout, "array_add::value=%s,array len=%d\n", data, array_len(&sk));
  }
  array_traverse(&sk, test_array_traverse_cb);
  for (size_t i = 0; i < n; i++)
  {
    char *data = array_del(&sk, i);
    fprintf(stdout, "array_del::value=%s,array len=%d\n", data, array_len(&sk));
    if (data)
    {
      free(data);
    }
  }
}
#endif