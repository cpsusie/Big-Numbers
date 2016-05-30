#include "pch.h"

int fstat64(FILE *f, struct _stati64 &st) {
  return _fstati64(_fileno(f) ,&st);
}

struct _stati64 &FSTAT64(FILE *f, struct _stati64 &st) {
  if(fstat64(f, st) < 0) {
    throwErrNoOnSysCallException(_T("fstat64"));
  }
  return st;
}

struct _stati64 FSTAT64(FILE *f) {
  struct _stati64 st;
  FSTAT64(f, st);
  return st;
}
