#include "pch.h"
#include "MemoryPool.h"

void *trimalloc(size_t size) {
  void *memptr = malloc(size);
  if (memptr == NULL) {
    throwException(_T("Out of memory"));
  }
  return memptr;
}

void trifree(void  *memptr) {
  free(memptr);
}

