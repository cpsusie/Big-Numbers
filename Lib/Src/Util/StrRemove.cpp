#include "pch.h"

TCHAR *strRemove(TCHAR *s, TCHAR ch) {
  TCHAR *d = s;
  for(TCHAR *t = s; *t; t++) {
    if(*t != ch) {
      *(d++) = *t;
    }
  }
  *d = 0;
  return s;
}
