#include "pch.h"

TCHAR *FGETS(TCHAR *line, size_t n, FILE *f) {
  TCHAR *l = _fgetts(line, n, f);
  TCHAR *nl;
 
  if((l != NULL) && ((nl = _tcschr(l,_T('\n'))) != NULL)) {
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
