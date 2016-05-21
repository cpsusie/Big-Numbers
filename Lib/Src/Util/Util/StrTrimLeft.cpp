#include "pch.h"

TCHAR *strTrimLeft(TCHAR *s) {
  for(_TUCHAR *f = (_TUCHAR*)s; _istspace(*f); f++);
  if((TCHAR*)f != s) {
    _tcscpy(s, (TCHAR*)f);
  }
  return s;
}
