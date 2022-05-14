/*************************************************************************
    > File Name: context_demo.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Sun 08 May 2022 08:53:02 AM UTC
 ************************************************************************/

#include <stdio.h>
#include "bitcask.h"
#include "schema.h"
#include "log.h"
int main(int argc, char *argv[])
{

  bitcask bk;
  int ret = bitcask_open(&bk, argv[1]);
  int n = atoi(argv[2]);
  for (size_t i = 0; i < n; i++)
  {
    char tmp[256] = {'\0'};
    snprintf((char *)&tmp, 256, "%d-schema", i);
    ret = bitcask_create_schema(&bk, (char *)&tmp);

    void *schema_ptr = bitcask_fetch_schema(&bk,(char *)&tmp);
    if(schema_ptr !=NULL) {
      schema *m = (schema *)schema_ptr;
      logi("name=%s,cnt=%d",m->meta->name,m->meta->data_file_cnt);
    }
  }
  for (size_t i = 0; i < n; i++)
  {
    char schema_buf[256] = {'\0'};
    snprintf((char *)&schema_buf, 256, "%d-schema", i);

    char key_buf[256] = {'\0'};
    snprintf((char *)&key_buf, 256, "%d-key", i);
    size_t key_sz = strlen((char *)&key_buf);

    ret = bitcask_put(&bk, (char *)&schema_buf, (char *)&key_buf, key_sz, i, sizeof(int));

  }
  for (size_t i = 0; i < n; i++)
  {
    char schema_buf[256] = {'\0'};
    snprintf((char *)&schema_buf, 256, "%d-schema", i);

    char key_buf[256] = {'\0'};
    snprintf((char *)&key_buf, 256, "%d-key", i);
    size_t key_sz = strlen((char *)&key_buf);

    void *ptr = bitcask_get(&bk, (char *)&schema_buf, (char *)&key_buf, key_sz);
    if (ptr != NULL)
    {
      free(ptr);
    }
  }
  bitcask_close(&bk);
}