#include "pch.h"

TCHAR *strReplace(TCHAR *dst, const TCHAR *src, const TCHAR *from, const TCHAR *to) {
  TCHAR *d = dst;
  const size_t fromlen = _tcsclen(from);
  if(fromlen == 0) {
    _tcscpy(dst,src);
    return dst;
  }
  const size_t tolen = _tcsclen(to);
  for(const TCHAR *s = src; *s;) {
    if(_tcsnccmp(s, from, fromlen) == 0) {
      _tcsncpy(d, to, tolen);
      s += fromlen;
      d += tolen;
    } else {
      *(d++) = *(s++);
    }
  }
  *d = 0;
  return dst;
}
