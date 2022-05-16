/*************************************************************************
    > File Name: c_hash_map.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Tue 22 Mar 2022 08:21:59 AM UTC
 ************************************************************************/
#ifndef _HASHMAP_H
#define _HASHMAP_H
#include "utils.h"
typedef struct hashmap
{
  size_t max_count;
   _Atomic(size_t) cur_count;
  void **ptr;
  hashmap_hash_cb hash_cb;
  hashmap_key_compare_cb cmp_cb;
  hashmap_free_cb key_free_cb;
  hashmap_free_cb val_free_cb;
} hashmap;

void hashmap_set_free_cb( hashmap *d,hashmap_free_cb key_free_cb,hashmap_free_cb val_free_cb);
int hashmap_init(hashmap *d, size_t max_count, hashmap_hash_cb hash_cb,hashmap_key_compare_cb cmp_cb);
hashmap *hashmap_alloc(size_t max_count,hashmap_hash_cb hash_cb,hashmap_key_compare_cb cmp_cb);
//void *hashmap_put(hashmap *d, char *key, void *val);

int hashmap_put(hashmap *d, void *key,size_t key_sz, void *value,size_t value_sz);
void *hashmap_get(hashmap *d, void *key,size_t key_sz);
int hashmap_del(hashmap *d, void *key,size_t key_sz);
int hashmap_traverse(hashmap *d,hashmap_traverse_cb traverse_cb);
int hashmap_deinit(hashmap *d);
void hashmap_destroy(hashmap *d);

inline static size_t hashmap_count(hashmap *d) {
  if(d!=NULL) {
    return d->cur_count;
  }
  return -1;
}

inline static bool hashmap_empty(hashmap *d) {
  if(d->cur_count <=0) {
    return false;
  }
  return true;
}
#endif
