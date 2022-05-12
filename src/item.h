/*************************************************************************
    > File Name: item.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Thu 12 May 2022 02:46:15 AM UTC
 ************************************************************************/

#ifndef _ITEM_H
#define _ITEM_H
#include "utils.h"
typedef struct item {
   uint32_t fid;
   uint64_t offset;
   size_t  size;
}item;
item *item_alloc(uint32_t fid,int64_t offset,size_t size) {
   item *it= (item *)calloc(1,sizeof(item));

   assert(it !=NULL);

     it->fid=fid;
     it->offset=offset;
     it->size = size;
  
   return it;
}

void item_destroy(item *it) {
   if(it) {
      free(it);
   }
}
#endif
