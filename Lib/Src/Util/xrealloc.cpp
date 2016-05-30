#include "pch.h"
#include <malloc.h>

void *xrealloc(void *q, unsigned int n) {
  void *p = realloc(q,n);
  if(!p) {
    throwException(_T("out of memory"));
  }
  return p;
}

