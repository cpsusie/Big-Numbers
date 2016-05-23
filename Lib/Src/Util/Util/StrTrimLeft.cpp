#include "pch.h"

TCHAR *strTrimLeft(TCHAR *s) {
  _TUCHAR *f;
  for(f = (_TUCHAR*)s; _istspace(*f); f++);
  if((TCHAR*)f != s) {
    _tcscpy(s, (TCHAR*)f);
  }
  return s;
}
