#include "pch.h"
#include <io.h>

int setFileMode(FILE *f, int mode) {
  int oldMode = _setmode(_fileno(f), mode);
  if(oldMode == -1) {
    throwErrNoOnSysCallException(_T("_setmode"));
  }
  return oldMode;
}

