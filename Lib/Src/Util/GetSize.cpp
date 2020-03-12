#include "pch.h"
#include <io.h>

INT64 GETSIZE(FILE *f) {
  INT64 result = _filelengthi64(_fileno(f));
  if(result < 0) {
    throwErrNoOnSysCallException(_T("_filelengthi64"));
  }
  return result;
}

