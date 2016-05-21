#include "pch.h"

int stat64(const String &name, struct _stati64 &st) {
  return _tstati64(name.cstr(), &st);
}

struct _stati64 &STAT64(const String &name, struct _stati64 &st) {
  if(stat64(name, st) < 0) {
    throwErrNoOnNameException(name);
  }
  return st;
}

struct _stati64 STAT64(const String &name) {
  struct _stati64 st;
  STAT64(name, st);
  return st;
}
