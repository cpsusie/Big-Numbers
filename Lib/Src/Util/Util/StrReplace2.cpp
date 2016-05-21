#include "pch.h"

TCHAR *strReplace(TCHAR *dst, const TCHAR *src, TCHAR from, const TCHAR *to) {
  TCHAR *d = dst;
  int tolen = _tcsclen(to);
  for(const TCHAR *s = src; *s; s++) {
    if(*s == from) {
      _tcscpy(d, to);
      d += tolen;
    } else {
      *(d++) = *s;
    }
  }
  *d = 0;
  return dst;
}
