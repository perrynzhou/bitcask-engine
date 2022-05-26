/*************************************************************************
    > File Name: kv_value.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Thu 12 May 2022 04:59:18 AM UTC
 ************************************************************************/

#ifndef _KV_VALUE_H
#define _KV_VALUE_H
#include "utils.h"

typedef struct kv_value
{
  uint32_t crc;
  uint32_t k_sz;
  uint32_t v_sz;
  char kv[0];
} kv_value;
kv_value *kv_value_alloc(void *key, size_t key_size, void *value, size_t value_size);
#endif
