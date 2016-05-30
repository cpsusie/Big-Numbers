#include "pch.h"

int fstat(FILE *f, struct _stat &st) {
  return _fstat(_fileno(f) ,&st);
}

struct _stat &FSTAT(FILE *f, struct _stat &st) {
  if(fstat(f, st) < 0) {
    throwErrNoOnSysCallException(_T("fstat"));
  }
  return st;
}

struct _stat FSTAT(FILE *f) {
  struct _stat st;
  FSTAT(f, st);
  return st;
}
