#include "pch.h"

#define LINESIZE 16

#define FPRINTLINE(f) _ftprintf(f,_T("%s\n"), spaceString(60, _T('-')).cstr())

void hexdump(const void *data, size_t size, FILE *f) {
  fprintf(f,"hexdump(size=%I64u)\n",(UINT64)size);
  const BYTE *p = (const BYTE*)data;
  FPRINTLINE(f);
  for(size_t i = 0; i < size; i += LINESIZE) {
    fprintf(f,"%p:",p+i);
    size_t j;
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
  FPRINTLINE(f);
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
