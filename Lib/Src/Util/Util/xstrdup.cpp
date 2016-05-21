#include "pch.h"
#include <malloc.h>

TCHAR *xstrdup(const TCHAR *str) {
  TCHAR *p = _tcsdup(str);
  if(!p) {
    throwException(_T("out of memory"));
  }
  return p;
}
