#include "pch.h"

FILE *fopen(const String &name, const String &mode ) {
  return _tfopen(name.cstr(), mode.cstr());
}

FILE *FOPEN(const TCHAR *name, const TCHAR *mode) {
  FILE *f = fopen(name, mode);
  if(f == NULL) {
    throwErrNoOnNameException(name);
  }
  return f;
}

FILE *FOPEN(const String &name, const String &mode) {
  return FOPEN(name.cstr(), mode.cstr());
}

