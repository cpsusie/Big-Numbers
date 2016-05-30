#include "pch.h"
#include <io.h>

void PIPE(int *pipe, unsigned int size, int textMode) {
  if(_pipe(pipe,size, textMode) < 0) {
    throwErrNoOnSysCallException(_T("_pipe"));
  }
}

