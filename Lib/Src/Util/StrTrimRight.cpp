#include "pch.h"

TCHAR *strTrimRight(TCHAR *s) {
  if(*s == 0) {
    return s;
  }

  _TUCHAR *t,*l;
  for(l = t = (_TUCHAR*)s; *t; t++) {
    if(!_istspace(*t)) {
      l = t;
    }
  }

  if(_istspace(*l))  {
    l[0] = 0;
  } else {
    l[1] = 0;
  }
  return s;
}

