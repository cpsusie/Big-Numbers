#include "stdafx.h"
#include <direct.h>
#include "util.h"

static void xmkdir(char *name) {
  for(char *sl = strchr(name, '\\'); sl; sl = strchr(sl+1, '\\')) {
    *sl = 0;
    _mkdir(name);
    *sl = '\\';
  }
  _mkdir(name);
}

FILE *mkfopen(const char *name, const char *mode) {
  char tmpName[1024];
  char *sl;
  strcpy(tmpName, name);
  if(sl = strchr(tmpName, '\\')) {
    *sl = 0;
    xmkdir(tmpName);
    *sl = '\\';
  }
  return FOPEN(tmpName, mode) ;
}

FILE *mkfopen(const string &name, const string &mode) {
  return mkfopen(name.c_str(), mode.c_str());
}

FILE *MKFOPEN(const char *name, const char *mode) {
  FILE *f = mkfopen(name, mode);
  if(f == NULL) {
    throwErrNoOnNameException(name);
  }
  return f;
}

FILE *MKFOPEN(const string &name, const string &mode) {
  return MKFOPEN(name.c_str(), mode.c_str());
}
