#include "pch.h"


//#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

ULONG strHash(const TCHAR * const &s) {
  ULONG i = 0;

  for(const _TUCHAR *cp = (const _TUCHAR*)s; *cp;) {
    i = (i * 117) ^ *(cp++);
  }
  return i;
}

int strHashCmp(const TCHAR * const &e1, const TCHAR * const &e2) {
  return _tcscmp(e1, e2);
}

