#include "pch.h"
#include <io.h>

void DUP2(int fd1, int fd2) {
  if(_dup2(fd1, fd2) < 0) {
    throwErrNoOnSysCallException(_T("dup2"));
  }
}
