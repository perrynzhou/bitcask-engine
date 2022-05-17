/*************************************************************************
    > File Name: utils.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Tue 17 May 2022 08:03:09 AM UTC
 ************************************************************************/

#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include "utils.h"
int traverse_schema_name(const char *db_home, array *arr)
{
  DIR *dir = opendir(db_home);
  if (!dir)
  {
    return -1;
  }
  struct dirent *ent;
  size_t i = 0;
  while ((ent = readdir(dir)) != NULL)
  {
    if (ent->d_type == DT_DIR)
    {
      char *schema_name = strdup(ent->d_name);
      if(strncmp(schema_name,".",1) ==0 || strncmp(schema_name,"..",2) ==0) {
         continue;
      }
      array_add(arr, schema_name, i);
      i++;
    }
  }
  return 0;
}
#ifdef UTILS_TEST
int utils_array_traverse_cb(void *arg)
{
  char *v = (char *)arg;
  fprintf(stdout, "traverse:value=%s\n", v);
  return 0;
}
int main() {
   array *arr = array_alloc(32,true);
   int ret = traverse_schema_name("/tmp/db",arr);
   array_traverse(arr,utils_array_traverse_cb);
   array_destroy(arr);
}
#endif