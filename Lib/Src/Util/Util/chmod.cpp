#include "pch.h"
#include <io.h>

int chmod(const String &name, int mode) {
  return _tchmod(name.cstr(), mode);
}

void CHMOD(const String &name, int mode) {
  if(chmod(name, mode) < 0) {
    throwErrNoOnNameException(name);
  }
}
