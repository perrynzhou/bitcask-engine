/*************************************************************************
    > File Name: object.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Sat 07 May 2022 06:11:21 AM UTC
 ************************************************************************/

#include "object.h"
#include "hash.h"
uint32_t object_hash(object *obj, int *ret)
{
  if (!obj)
  {
    *ret = -1;
    return 0;
  }
  *ret = 0;
  char *data = (char *)obj->data;
  size_t len = strlen(data);
  return hash_fnv1a_64(data, len);
}
object *object_alloc(void *data, size_t len)
{
  object *obj = NULL;
  if (data)
  {
    obj = (object *)calloc(1, sizeof(object) + len);
    assert(obj != NULL);
    memcpy(&obj->data, data, len);
    obj->len = len;
  }
  return obj;
}
void object_destroy(object *obj)
{
  if (obj)
  {
    free(obj);
    obj = NULL;
  }
}