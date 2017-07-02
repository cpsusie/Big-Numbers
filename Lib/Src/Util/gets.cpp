#include "pch.h"

TCHAR *GETS(TCHAR *line) {
  TCHAR *l = _fgetts(line,256,stdin);
  TCHAR *nl;

  if((l != NULL) && ((nl = _tcschr(l, _T('\n'))) != NULL)) {
    *nl = 0;
  }
  return l;
}
