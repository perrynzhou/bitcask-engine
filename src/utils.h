/*************************************************************************
    > File Name: utils.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Sat 07 May 2022 06:19:00 AM UTC
 ************************************************************************/

/*************************************************************************
    > File Name: c_utils.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Mon 21 Mar 2022 11:50:59 AM UTC
 ************************************************************************/

#ifndef _C_UTILS_H
#define _C_UTILS_H
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/uio.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/param.h>
typedef uint32_t (*hashmap_hash_cb)(void *, size_t);
typedef int (*hashmap_traverse_cb)(void *,void *);
typedef int (*hashmap_key_compare_cb)(void *,void *,size_t);

typedef void (*hashmap_free_cb)(void *);
typedef int (*key_compare)(void *,void *,size_t);
#endif