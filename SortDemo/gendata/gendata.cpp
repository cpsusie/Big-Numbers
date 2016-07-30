// gendata.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>

static unsigned long randlong() {
  return (rand() << 16) + rand();
}

static char *randstr(char *dst, int len) {
  for(int i = 0; i < len; i++)
    dst[i] = 'a' + rand() % ('z'-'a');
  dst[i] = 0;
  return dst;
}

static void usage() {
  fprintf(stderr,"usage:gendata [-s[length]] [-ncount]\n");
  exit(-1);
}

typedef enum {
  TYPE_INT,
  TYPE_STRING
} datatype;

int main(int argc, char **argv) {
  char *cp;
  datatype type = TYPE_INT;

  int n = 300;
  int type
  for(argv++; *argv && *(cp=*argv) == '-'; argv++) {
    for(
  argv++;
  if(*argv) {
    if(sscanf(*argv,"%lu",&n) != 1) usage();
  }
  if(n <= 0) usage();

  for(int i = 0; i < n; i++)
    printf("line%08X\n",randlong());
  return 0;
}
