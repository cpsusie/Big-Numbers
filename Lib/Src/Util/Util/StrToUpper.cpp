#include "pch.h"
#include <ctype.h>

TCHAR *strToUpperCase(TCHAR *s) {
  for(_TUCHAR *t = (_TUCHAR*)s; *t; t++) {
    if(_istascii(*t) && _istlower(*t)) {
      *t = _totupper(*t);
    }
  }
  return s;
}
