/*************************************************************************
    > File Name: utils.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Tue 17 May 2022 08:03:09 AM UTC
 ************************************************************************/

#include <stdio.h>
#include <dirent.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include "utils.h"
#include "hashmap.h"
#include "hash.h"
#include "log.h"
#define MAX_DATA_FILE (65525)
#define  MAX_BUF_SZ  (1024)

array  *search_files(const char *schema_dir,bool is_file)
{
  array *results = NULL;
  if(schema_dir) {
  DIR *dir = opendir(schema_dir);
  if (!dir)
  {
    return results;
  }
  struct dirent *ent;
  int i=0;
  int flag = (is_file)?DT_REG:DT_DIR;
  while ((ent = readdir(dir)) != NULL)
  {
      char *file_name = strdup(ent->d_name);
      if (strncmp(file_name, ".", 1) == 0 || strncmp(file_name, "..", 2) == 0)
      {
        continue;
      }
      if(ent->d_type==flag) {
        array_add(results, file_name, i);
        i++;
      }
  }
  }
  return results;
}
hashmap *load_schema_files(const char *db_home)
{
  DIR *dir = opendir(db_home);
  if (!dir)
  {
    return NULL;
  }
  struct dirent *ent;
  size_t i = 0;
  hashmap *schema_data_files = hashmap_alloc(MAX_DATA_FILE, (hashmap_hash_cb)&hash_fnv1_32, (hashmap_key_compare_cb)&memcmp);
  array *arr = array_alloc(256, true);
  while ((ent = readdir(dir)) != NULL)
  {
    if (ent->d_type == DT_DIR)
    {
      char *schema_name = strdup(ent->d_name);
      if (strncmp(schema_name, ".", 1) == 0 || strncmp(schema_name, "..", 2) == 0)
      {
        continue;
      }
      array_add(arr, schema_name, i);
      slog_info("search schema name=%s", schema_name);
      i++;
    }
  }
  for (i = 0; i < array_len(arr); i++)
  {
    char *schema_name = (char *)array_value(arr, i);
    
    char search_path[MAX_BUF_SZ] = {'\0'};
    snprintf((char *)&search_path, MAX_BUF_SZ, "%s/%s", db_home, schema_name);
    slog_info("open schema =%s", search_path);
    DIR *tmp_dir = opendir((char *)&search_path);
    int j = 0;
    if (tmp_dir)
    {
      array *data_files = array_alloc(256, true);
      assert(data_files != NULL);

      while ((ent = readdir(tmp_dir)) != NULL)
      {
        if (ent->d_type == DT_REG && (strstr(ent->d_name, ".data") != NULL||strstr(ent->d_name, ".wal") != NULL))
        {
          snprintf((char *)&search_path, MAX_BUF_SZ, "%s/%s/%s", db_home, schema_name, ent->d_name);
          array_add(data_files, (char *)&search_path, j);
          slog_info("open schema=%s   file-%d=%s",schema_name, j, search_path);
          j++;
        }
      }
      if (array_len(data_files) > 0)
      {
        hashmap_put(schema_data_files, schema_name, strlen(schema_name), &data_files, sizeof(void *));
      }
      else
      {
        array_destroy(data_files);
      }
    }
  }
  array_destroy(arr);
  return schema_data_files;
}
#ifdef UTILS_TEST
int utils_array_traverse_cb(void *arg)
{
  char *v = (char *)arg;
  fprintf(stdout, "traverse:value=%s\n", v);
  return 0;
}
int main()
{
    uint16_t n_log_flags = SLOG_ERROR | SLOG_NOTAG;

  slog_init("bitcask-engine", n_log_flags, 0);
  slog_enable(SLOG_FLAGS_ALL);
  slog_config cfg;
  slog_config_get(&cfg);
    cfg.n_trace_tid = 1;
     slog_config_set(&cfg);
  /*
  array *arr = array_alloc(32, true);
  int ret = traverse_schema_name("/tmp/db", arr);
  array_traverse(arr, utils_array_traverse_cb);
  array_destroy(arr);
  */
  load_schema_files("/tmp/db");
}
#endif