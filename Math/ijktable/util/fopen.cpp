#include "stdafx.h"

FILE *fopen(const string &name, const string &mode ) {
  return fopen(name.c_str(), mode.c_str());
}

FILE *FOPEN(const char *name, const char *mode) {
  FILE *f = fopen(name, mode);
  if(f == NULL) {
    throwErrNoOnNameException(name);
  }
  return f;
}

FILE *FOPEN(const string &name, const string &mode) {
  return FOPEN(name.c_str(), mode.c_str());
}

