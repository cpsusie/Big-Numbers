#include "pch.h"

size_t FREAD(void *buffer, size_t size, size_t count, FILE *f) {
  size_t n = fread(buffer, size, count, f);
  if(n != count ) {
    int error = ferror(f);
    if(error) {
      throwException(_T("fread:error %d"), error);
    }
  }
  return n;
}
