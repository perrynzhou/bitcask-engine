/*************************************************************************
    > File Name: conf.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Thu 12 May 2022 02:49:15 AM UTC
 ************************************************************************/

#ifndef _CONF_H
#define _CONF_H
#include "utils.h"
typedef struct conf
{
   char  *db_home;
   size_t max_data_file_size;
   size_t max_key_size;
   size_t max_value_size;
   int   sync;
   int  auto_recovery;
   int32_t db_vsersion;
} conf;
conf *conf_create(const char *conf_file);
void conf_dum(conf *m_conf);
void conf_destroy(conf *mconf);
#endif
