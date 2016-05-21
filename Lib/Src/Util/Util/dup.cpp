#include "pch.h"
#include <io.h>

int DUP(int fd) {
  int result = _dup(fd);
  if(result < 0) {
    throwErrNoOnSysCallException(_T("dup"));
  }
  return result;
}

