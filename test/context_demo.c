/*************************************************************************
    > File Name: context_demo.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Sun 08 May 2022 08:53:02 AM UTC
 ************************************************************************/

#include <stdio.h>
#include "context.h"
int main() {
  context *ctx = context_open("/tmp/self_data");
  
  context_close(ctx);
}