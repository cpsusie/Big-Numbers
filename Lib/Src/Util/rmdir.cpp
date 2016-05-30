#include "pch.h"
#include <direct.h>

int rmdir(const String &name) {
  return _trmdir(name.cstr());
}

void RMDIR(const String &name) {
  if(rmdir(name) < 0) {
    throwErrNoOnNameException(name);
  }
}
