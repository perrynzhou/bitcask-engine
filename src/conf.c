/*************************************************************************
    > File Name: conf.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Thu 12 May 2022 02:52:26 AM UTC
 ************************************************************************/

#include "conf.h"
#include <assert.h>
#include <fcntl.h>
#include <jansson.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
conf *conf_create(const char *conf_file) {
  conf *m_conf = NULL;
  char *buf = NULL;
  json_t *root = NULL;
  size_t n = 0;
  size_t readn = 0;
  int fd = -1;
  if (conf_file == NULL) {
    goto out;
  }
  if (access(conf_file, F_OK) != 0) {
    goto out;
  }
  n = (1 << 10) * (1 << 10);
  fd = open(conf_file, O_RDONLY);
  buf = (char *)malloc(n);
 readn = read(fd, buf, n);
  if (readn <= 0) {
    goto out;
  }
  close(fd);
  json_error_t error;

  root = json_loads(buf, 0, &error);
  if (root != NULL) {
    m_conf = (conf *)malloc(sizeof(conf));
    assert(m_conf != NULL);
    json_t *json_max_data_file_size =
        json_object_get(root, "max_data_file_size");
    json_t *json_max_key_size = json_object_get(root, "max_key_size");
    json_t *json_max_value_size = json_object_get(root, "max_value_size");
    json_t *json_sync = json_object_get(root, "sync");
    json_t *json_auto_recovery = json_object_get(root, "auto_recovery");
    json_t *json_db_vsersion = json_object_get(root, "db_vsersion");
    if (!json_max_data_file_size || !json_max_key_size ||
        !json_max_value_size || !json_auto_recovery || !json_db_vsersion) {
      free(m_conf);
      m_conf = NULL;
      goto out;
    }
    m_conf->max_data_file_size = json_integer_value(json_max_data_file_size);
    m_conf->max_key_size = json_integer_value(json_max_key_size);
    m_conf->max_value_size = json_integer_value(json_max_value_size);
    m_conf->sync = json_integer_value(json_sync);
    m_conf->auto_recovery = json_integer_value(json_auto_recovery);
    m_conf->db_vsersion = json_integer_value(json_db_vsersion);
  }
out:
  if (buf != NULL) {
    free(buf);
  }
  if (root != NULL) {
    json_decref(root);
  }
  return m_conf;
}
void conf_destroy(conf *mconf) {
  if (mconf != NULL) {
    free(mconf);
    mconf = NULL;
  }
}