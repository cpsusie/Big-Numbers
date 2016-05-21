#include "pch.h"
#include <malloc.h>

void *xmalloc(unsigned int n) {
  void *p = malloc(n);
  if(!p) {
    throwException(_T("out of memory"));
  }
  memset(p,0,n);
  return p;
}
