#include "pch.h"

//#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

ULONG striHash(const TCHAR * const &s) {
  ULONG i = 0;

  for(const _TUCHAR *cp = (const _TUCHAR*)s; *cp; cp++) {
    const _TUCHAR ch = _istlower(*cp) ? _totupper(*cp) : *cp;
    i = (i * 117) ^ ch;
  }
  return i;
}

int striHashCmp(const TCHAR * const &e1, const TCHAR * const &e2) {
  return _tcsicmp(e1, e2);
}

