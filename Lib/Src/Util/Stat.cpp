#include "pch.h"

int stat(const String &name, struct _stat &st) {
  return _tstat(name.cstr() ,&st);
}

struct _stat &STAT(const String &name, struct _stat &st) {
  if(stat(name, st) < 0) {
    throwErrNoOnNameException(name);
  }
  return st;
}

struct _stat STAT(const String &name) {
  struct _stat st;
  STAT(name, st);
  return st;
}
