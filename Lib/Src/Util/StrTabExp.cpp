#include "pch.h"
#include <String.h>

TCHAR *strTabExpand(TCHAR *dst, const TCHAR *src, int tabSize, TCHAR subst) {
  size_t l = _tcsclen(src), j = 0;

  for(size_t i = 0; i < l; i++) {
    if(src[i] != _T('\t')) {
      dst[j++] = src[i];
    } else {
      int numSpace = tabSize - j % tabSize;
      TMEMSET(dst + j,_T(' '), numSpace);
      if(numSpace > 0) {
        dst[j] = subst;
      }
      j += numSpace;
    }
  }
  dst[j] = 0;
  return dst ;
}
