#include "pch.h"

int unlink(const String &name) {
  return _tunlink(name.cstr());
}

void UNLINK(const String &name) {
  if(unlink(name) < 0) {
    throwErrNoOnNameException(name);
  }
}
