#include "stdafx.h"
#include "stormacs.h"
#include "io.h"

FILE *DFILE;

void *xmalloc(size_t n) {
  void *p = malloc(n);
  if(!p) {
    panic("out of memory");
  }
  memset(p, 0, n);
  return p;
}

void *xrealloc(void *q, size_t n) {
  void *p = realloc(q, n);
  if(!p) {
    panic("out of memory");
  }
  return p;
}

char *xstrdup(const char *str) {
  char *p = _strdup(str);
  if(!p) {
    panic("out of memory");
  }
  return p;
}

void xfree(void *p) {
  free(p);
}
