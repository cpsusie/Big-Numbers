#include "pch.h"

TCHAR *strReplace(TCHAR *s, TCHAR from, TCHAR to ) {
  for(TCHAR *t=s; *t; t++) {
    if(*t == from) {
      *t = to;
    }
  }
  return s;
}
