/*************************************************************************
    > File Name: object.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Sat 07 May 2022 06:11:17 AM UTC
 ************************************************************************/

#ifndef _OBJECT_H
#define _OBJECT_H
#include "utils.h"
typedef struct object
{
  size_t len;
  char data[0];
} object;
object *object_alloc(void *data, size_t len);
void object_destroy(object *obj);
uint32_t object_hash(object *obj, int *ret);
inline size_t object_size(object *obj)
{
  if (obj != NULL)
  {
    return obj->len;
  }
  return 0;
}

#endif
