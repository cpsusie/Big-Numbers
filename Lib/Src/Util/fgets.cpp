#include "pch.h"

TCHAR *FGETS(TCHAR *line, size_t n, FILE *f) {
  TCHAR *l = _fgetts(line, (int)n, f);
  TCHAR *nl;

  if((l != nullptr) && ((nl = _tcschr(l,_T('\n'))) != nullptr)) {
    *nl = 0;
    if(nl > l) {
      nl--;
      if(*nl == _T('\r')) {
        *nl = 0;
      }
    }
  }
  return l;
}
