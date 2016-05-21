#include "pch.h"
#include <io.h>

__int64 GETPOS(FILE *f) {
  fpos_t result;
  if(fgetpos(f, &result) != 0) {
    throwErrNoOnSysCallException(_T("fgetpos"));
  }
  return result;
}

