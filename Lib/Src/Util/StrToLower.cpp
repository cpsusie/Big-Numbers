#include "pch.h"
#include <ctype.h>

TCHAR *strToLowerCase(TCHAR *s) {
  for(_TUCHAR *t = (_TUCHAR*)s; *t; t++) {
    if(_istascii(*t) && _istupper(*t)) {
      *t = _totlower(*t);
    }
  }
  return s;
}

