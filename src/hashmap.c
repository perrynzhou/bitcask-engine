/*************************************************************************
    > File Name: c_hash_map.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Tue 22 Mar 2022 08:21:44 AM UTC
 ************************************************************************/

#ifndef _HASH_MAP_H
#define _HASH_MAP_H
#include "hashmap.h"
#include <stdatomic.h>
typedef struct member_pair
{
  void *key;
  void *val;
  void *next;
} member_pair;

static member_pair *member_pair_alloc(void *key, size_t key_sz, void *val, size_t val_sz)
{
  member_pair *pair = NULL;
  if (key != NULL && val != NULL && key_sz > 0 && val_sz > 0)
  {
    pair = (member_pair *)calloc(1, sizeof(member_pair));
    assert(pair != NULL);
    pair->next = NULL;
    pair->key = calloc(1, sizeof(char) * key_sz);
    assert(pair->key != NULL);
    pair->val = calloc(1, sizeof(char) * val_sz);
    assert(pair->val != NULL);
    memcpy(pair->key, key, key_sz);
    memcpy(pair->val, val, val_sz);
  }
  return pair;
}
static uint64_t hashmap_jump_consistent(uint64_t key, size_t num_buckets)
{

  int64_t b = -1, j = 0;
  size_t value = 0;
  uint64_t num_bucket = (uint64_t)num_buckets;
  while (j < num_bucket)
  {
    b = j;
    key = key * 2862933555777941757ULL + 1;
    j = (int64_t)((b + 1) * ((double)(1LL << 31) / (double)((key >> 33) + 1)));
  }
  value = (b < 0) ? (~b + 1) : b;
  return value;
}
inline static member_pair *hashmap_search(hashmap *d, uint32_t index, void *key, size_t key_sz)
{
  member_pair *tmp = (member_pair *)d->ptr[index];
  member_pair *dst = NULL;

  while (tmp != NULL)
  {
    member_pair *next = (member_pair *)tmp->next;
    if (d->cmp_cb(tmp->key, key, key_sz) == 0)
    {
      dst = tmp;
      break;
    }
    tmp = next;
  }
  return dst;
}

inline static uint32_t hashmap_get_index(hashmap *d,void *key,size_t key_sz) {
    uint32_t hash_key = d->hash_cb((char *)key, key_sz);
    uint32_t index = hashmap_jump_consistent(hash_key, d->max_count);
    return index;
}
inline void hashmap_set_free_cb(hashmap *d, hashmap_free_cb key_free_cb, hashmap_free_cb val_free_cb)
{
  if (d != NULL)
  {
    d->key_free_cb = key_free_cb;
    d->val_free_cb = val_free_cb;
  }
}
static void c_member_pair_free(member_pair *pair, hashmap_free_cb key_free, hashmap_free_cb val_free)
{
  if (pair != NULL)
  {
    if (key_free != NULL)
    {
      key_free(pair->key);
      pair->key = NULL;
    }
    if (val_free != NULL)
    {
      val_free(pair->val);
      pair->val = NULL;
    }
    free(pair);
    pair = NULL;
  }
}

int hashmap_init(hashmap *d, size_t max_count, hashmap_hash_cb hash_cb, hashmap_key_compare_cb cmp_cb)
{
  if (d != NULL && max_count > 0 && hash_cb != NULL && cmp_cb != NULL)
  {
    d->ptr = (void **)calloc(max_count, sizeof(void **));
    assert(d->ptr != NULL);
    d->hash_cb = hash_cb;
    d->cmp_cb = cmp_cb;
    d->cur_count = 0;
    d->key_free_cb = NULL;
    d->val_free_cb = NULL;
    d->max_count=max_count;

    return 0;
  }
  return -1;
}
hashmap *hashmap_alloc(size_t max_count, hashmap_hash_cb hash_cb, hashmap_key_compare_cb cmp_cb)
{
  hashmap *hash_map = NULL;
  if (max_count > 0 && hash_cb != NULL && cmp_cb != NULL)
  {
    hash_map = (hashmap *)calloc(1, sizeof(hashmap));
    assert(hash_map != NULL);
    if (hashmap_init(hash_map, max_count, hash_cb, cmp_cb) != 0)
    {
      free(hash_map);
      hash_map = NULL;
    }
  }
  return hash_map;
}
int hashmap_put(hashmap *d, void *key, size_t key_sz, void *value, size_t value_sz)
{
  if (d != NULL && key != NULL && key_sz > 0 && value != NULL && value_sz > 0)
  {
    if (d->cur_count < d->max_count)
    {
      member_pair *pair = member_pair_alloc(key, key_sz+1, value, value_sz);
      assert(pair != NULL);
      uint32_t index = hashmap_get_index(d,key,key_sz);
      if (d->ptr[index] == NULL)
      {
        d->ptr[index] = pair;
      }
      else
      {
        pair->next = d->ptr[index];
        d->ptr[index] = pair;
      }
      atomic_fetch_add(&d->cur_count,1);
      return 0;
    }
  }
  return -1;
}

void *hashmap_get(hashmap *d, void *key, size_t key_sz)
{
  void *data = NULL;
  if (d != NULL && d->cur_count > 0)
  {
   uint32_t index = hashmap_get_index(d,key,key_sz);
    if (d->ptr[index] != NULL)
    {
      member_pair *dst = hashmap_search(d, index, key, key_sz);
      if (dst != NULL)
      {
        data = dst->val;
      }
    }
  }
  return data;
}
int hashmap_del(hashmap *d, void *key, size_t key_sz)
{
  if (d != NULL && key != NULL)
  {
    member_pair *dst = NULL;
   uint32_t index = hashmap_get_index(d,key,key_sz);
    if (d->ptr[index] != NULL)
    {
      member_pair *tmp = (member_pair *)d->ptr[index];
      member_pair *prev = NULL;
      while (tmp != NULL)
      {
        member_pair *next = (member_pair *)tmp->next;
        if (d->cmp_cb(tmp->key, key, key_sz) == 0)
        {
          dst = tmp;
          break;
        }
        prev = tmp;
        tmp = next;
      }

      if (dst != NULL)
      {
        if (prev == NULL)
        {
          d->ptr[index] = dst->next;
        }
        else
        {
          prev->next = dst->next;
        }
        c_member_pair_free(dst, d->key_free_cb, d->val_free_cb);
         d->cur_count--;
      }
    }
    return 0;
  }
  return -1;
}
int hashmap_traverse(hashmap *d,hashmap_traverse_cb traverse_cb)
{
   if(d !=NULL && d->cur_count>0)
   {
     for(size_t i=0;i<d->max_count;i++)
     {
        member_pair *tmp = (member_pair *)d->ptr[i];
        while(tmp !=NULL)
        {
           member_pair *next = (member_pair *)tmp->next;
           if(traverse_cb !=NULL) {
              traverse_cb(tmp->key,tmp->val);
           }
           tmp =next;
        }
     }
     return 0;
   }
   return -1;
}
int hashmap_deinit(hashmap *d)
{
   if(d !=NULL && d->cur_count>0)
   {
     for(size_t i=0;i<d->max_count;i++)
     {
        member_pair *tmp = (member_pair *)d->ptr[i];
        while(tmp !=NULL)
        {
           member_pair *next = (member_pair *)tmp->next;
           c_member_pair_free(tmp,d->key_free_cb,d->val_free_cb);
           tmp =next;
        }
     }
     return 0;
     
   }
   return -1;
}
void hashmap_destroy(hashmap *d)
{
  if(hashmap_deinit(d)!=-1)
  {
    free(d);
    d=NULL;
  }
}
#endif
