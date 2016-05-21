#include "pch.h"
#include <direct.h>

int chdir(const String &dir) {
  return _tchdir(dir.cstr());
}

void CHDIR(const String &dir) {
  if(chdir(dir) < 0) {
    throwErrNoOnNameException(dir);
  }
}
