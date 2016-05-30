#include "pch.h"
#include <String.h>

TCHAR *strTabExpand(TCHAR *dst, const TCHAR *src, int tabSize, TCHAR subst) {
  int l = _tcsclen(src), j = 0;

  for(int i=0; i < l; i++) {
    if(src[i] != _T('\t')) {
      dst[j++] = src[i];
    } else {
      int numSpace = tabSize - j % tabSize;
      MEMSET(dst + j,_T(' '), numSpace);
      if(numSpace > 0) {
        dst[j] = subst;
      }
      j += numSpace;
    }
  }
  dst[j] = 0;
  return dst ;
}
