#include "pch.h"

void FWRITE(const void *buffer, size_t size, size_t count, FILE *f) {
  size_t n = fwrite(buffer, size, count, f);
  if(n != count) {
    throwException(_T("fwrite:tried to write %d items, but only %d was written"), count, n);
  }
  fflush(f);
}
