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

  uint16_t n_log_flags = SLOG_ERROR | SLOG_NOTAG;

  slog_init("bitcask-engine", n_log_flags, 0);
  slog_enable(SLOG_FLAGS_ALL);
  slog_config cfg;
  slog_config_get(&cfg);
    cfg.n_trace_tid = 1;
     slog_config_set(&cfg);
  bitcask bk;
  int ret = bitcask_open(&bk, argv[1]);
  slog_info("bitcask_open ret=%d", ret);
  int n = atoi(argv[2]);
  char *schema_name =NULL;
  for (size_t i = 0; i < n; i++)
  {
    char tmp[256] = {'\0'};
    snprintf((char *)&tmp, 256, "%ld-schema", i);
   schema_name = strdup((char *)&tmp);
    ret = bitcask_create_schema(&bk, (char *)&tmp);

    void *schema_ptr = bitcask_fetch_schema(&bk, (char *)&tmp);
    if (schema_ptr != NULL)
    {
      schema *m = (schema *)schema_ptr;
      slog_info("name=%s,cnt=%ld", m->meta->name, m->meta->data_file_cnt);
    }
    if(i == 0) {
      break;
    }
  }
  size_t *keys = calloc(n,sizeof(size_t));
  for (size_t i = 0; i < n; i++)
  {

    keys[i]=rand();
    char key_buf[256] = {'\0'};
    snprintf((char *)&key_buf, 256, "%ld-key", keys[i]);
    size_t key_sz = strlen((char *)&key_buf);
    int value = rand()%64 +1;
    ret = bitcask_put(&bk, schema_name, (char *)&key_buf, key_sz, &value, sizeof(int));
    slog_info("bitcask_put ret=%d:: schema=%s,key=%s,value=%d", ret, schema_name, (char *)&key_buf,value);
  }
  for (size_t i = 0; i < n; i++)
  {
    

    char key_buf[256] = {'\0'};
    snprintf((char *)&key_buf, 256, "%ld-key", keys[i]);
    size_t key_sz = strlen((char *)&key_buf);

    int *ptr = (int *)bitcask_get(&bk, schema_name ,(char *)&key_buf, key_sz);
    if (ptr)
    {
      slog_info("bitcask_get:: schema=%s,key=%s,value=%d", schema_name, (char *)&key_buf, *ptr);
    }
  }
  bitcask_close(&bk);
}