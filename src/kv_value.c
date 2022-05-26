/*************************************************************************
    > File Name: kv_value.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Thu 26 May 2022 09:52:14 AM UTC
 ************************************************************************/

#include <stdio.h>
#include "crc.h"
#include "kv_value.h"
kv_value *kv_value_alloc(void *key, size_t key_size, void *value, size_t value_size)
{
  kv_value *val = NULL;
  if (value!=NULL)
  {
    val = (kv_value *)calloc(1, sizeof(kv_value) + key_size + value_size);
    assert(val != NULL);
    char *kv_data = (char *)val->kv;
    if(key && key_size>0)  {
        memcpy(kv_data, key, key_size);
        val->k_sz = key_size;
    }
    memcpy(kv_data + key_size, value, value_size);
    val->v_sz = value_size;
    val->crc = crc32(kv_data, key_size + value_size);
   
  }
  return val;
}