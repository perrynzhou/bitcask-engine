/*************************************************************************
    > File Name: log.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Mon 28 Mar 2022 12:12:43 PM UTC
 ************************************************************************/

#ifndef _LOG_H
#define _LOG_H
#define LOG_ERR 0
#define LOG_WARNING 1
#define LOG_INFO 2
#define LOG_DEBUG 3

typedef enum {
  LOG_STDIN = 0,
  LOG_STDOUT = 1,
  LOG_STDERR = 2,
  LOG_FILE = 3,
} log_type;

int log_init(log_type type, const char *file);
void log_set_level(int level);
void log_deinit();
int log_print(int lvl, const char *file, int line, const char *func,
              const char *fmt, ...);
#define loge(...) log_print(LOG_ERR, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define logw(...)                                                              \
  log_print(LOG_WARNING, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define logi(...) log_print(LOG_INFO, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define logd(...)                                                              \
  log_print(LOG_DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__)

#endif
