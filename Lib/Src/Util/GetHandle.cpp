#include "pch.h"
#include <io.h>

HANDLE getHandle(FILE *f) {
  return (HANDLE)_get_osfhandle(_fileno(f));
}

HANDLE GETHANDLE(FILE *f) {
  HANDLE h = getHandle(f);
  if(h == INVALID_HANDLE_VALUE) {
    throwErrNoOnSysCallException(_T("GETHANDLE"));
  }
  return h;
}
