#include "pch.h"

#define LINESIZE 16

void hexdump(const void *data, int size, FILE *f) {
  fprintf(f,"hexdump(size=%d)\n",size);
  unsigned char *p = (unsigned char*)data;
  fprintf(f,"------------------------------------------------------------\n");
  for(int i = 0; i < size; i += LINESIZE) {
    fprintf(f,"%p:",p+i);
    int j;
    for(j = i; j < i+LINESIZE && j < size; j++) {
      fprintf(f,"%02x ",p[j]);
    }
    for(; j < i+LINESIZE; j++) {
      fprintf(f,"   ");
    }
    fprintf(f,"      ");
    for(j = i; j < i+LINESIZE && j < size; j++) {
      fprintf(f,"%c ",isprint(p[j])?p[j]:'.');
    }
    fprintf(f,"\n");
  }
  fprintf(f,"------------------------------------------------------------\n");
}

String bytesToString(const void *data, int size) {
  unsigned char *p = (unsigned char*)data;
  char *tmp = new char[size*3 + 1];
  char *cp = tmp;
  
  for(int i = 0; i < size; i++, cp+=3) {
    sprintf(cp,"%02x ",p[i]);
  }
  String result = tmp;
  delete[] tmp;
  return result;
}
