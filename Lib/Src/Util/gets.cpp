#include "pch.h"

TCHAR *GETS(TCHAR *line) {
  TCHAR *l = _fgetts(line,256,stdin);
  TCHAR *nl;

  if((l != nullptr) && ((nl = _tcschr(l, _T('\n'))) != nullptr)) {
    *nl = 0;
  }
  return l;
}
