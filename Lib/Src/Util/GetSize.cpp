#include "pch.h"
#include <io.h>

__int64 GETSIZE(FILE *f) {
  __int64 result = _filelengthi64(_fileno(f));
  if(result < 0) {
    throwErrNoOnSysCallException(_T("_filelengthi64"));
  }
  return result;
}

