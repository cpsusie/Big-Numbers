#include "pch.h"
#include <direct.h>

String GETCWD() {
  TCHAR wd[_MAX_PATH];
  if(_tgetcwd(wd, ARRAYSIZE(wd)) == nullptr) {
    throwErrNoOnSysCallException(_T("getcwd"));
  }
  return wd;
}

