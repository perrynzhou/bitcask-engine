/*************************************************************************
    > File Name: entry.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Thu 12 May 2022 04:59:18 AM UTC
 ************************************************************************/

#ifndef _ENTRY_H
#define _ENTRY_H
#include "utils.h"
#include "crc.h"
typedef struct entry {
  uint32_t crc;
  uint32_t k_sz;
  uint32_t v_sz;
  char kv[0];
}entry;
inline entry *entry_alloc(void *key,size_t key_size,void *value,size_t value_size) {
  entry *et = NULL;
 if(key && value) {
      et = (entry *)calloc(1,sizeof(entry)+key_size+value_size);
  assert(et !=NULL);
    char  *kv_data = (char *)et->kv;
    memcpy(kv_data,key,key_size);
    memcpy(kv_data+key_size,value,value_size);
    et->crc = crc32(kv_data,key_size+value_size);
    et->k_sz = key_size;
    et->v_sz  = value_size;
 }
 return et;
}
#endif
