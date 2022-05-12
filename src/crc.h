/*************************************************************************
  > File Name: crc.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: 六  9/19 18:23:39 2020
 ************************************************************************/

#ifndef _CRC_H
#define _CRC_H
#include <stdint.h>
uint64_t crc64(const char *s, int l);
uint16_t crc16(const char *buf, int len);
uint32_t crc32(const char* s, int len);
#endif