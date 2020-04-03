#include "stdafx.h"

#ifndef TRACE_MEMORY

void *xmalloc(size_t n) {
  void *p = malloc(n);
  if(!p) {
    throwException("out of memory");
  }
  memset(p, 0, n);
  return p;
}

void *xrealloc(void *q, size_t n) {
  void *p = realloc(q, n);
  if(!p) {
    throwException("out of memory");
  }
  return p;
}

char *xstrdup(const char *str) {
  char *p = _strdup(str);
  if(!p) {
    throwException("out of memory");
  }
  return p;
}

void xfree(void *p) {
  free(p);
}

#else

void *xmalloc(size_t tsize, int count, const char *method, int line, const char *type) {
  const size_t n = tsize * count;
  void *p = malloc(n);
  if(!p) {
    throwException("out of memory");
  }
  memset(p, 0, n);
  debugLog("MALLOC:%p:%s(%d):%s(typesize=%d,count=%d, bytes=%u)\n"
          ,p,method,line,type,tsize,count,n
          );
  return p;
}
void  *xrealloc(void *q, size_t tsize, int count, const char *method, int line, const char *type) {
  const size_t n = tsize * count;
  void *p = realloc(q, n);
  if(!p) {
    throwException("out of memory");
  }
  debugLog("REALLOC:%p(%p):%s(%d):%s(typesize=%d,count=%d, bytes=%u)\n"
          ,q, p, method, line, type, tsize, count, n
          );
  debugLog("FREE:%p:%s(%d)\n", q, method, line);
  debugLog("MALLOC:%p:%s(%d):%s(typesize=%d,count=%d, bytes=%u)\n"
          ,p, method, line, type, tsize, count, n
  );
  return p;

}
char *xstrdup(const char *str, const char *method, int line) {
  char *p = _strdup(str);
  if(!p) {
    throwException("out of memory");
  }
  debugLog("STRDUP:%p:%s(%d)(\"%s\")\n", p, method, line, str);
  return p;
}
void xfree(void *p, const char *method, int line) {
  debugLog("FREE:%p:%s(%d)\n", p, method, line);
  free(p);
}

#endif // TRACE_MEMORY
