#include "pch.h"
#include <direct.h>

static void xmkdir(TCHAR *name) {
  for(TCHAR *sl = _tcschr(name, _T('\\')); sl; sl = _tcschr(sl+1, _T('\\'))) {
    *sl = 0;
    _tmkdir(name);
    *sl = _T('\\');
  }
  _tmkdir(name);
}

FILE *mkfopen(const TCHAR *name, const TCHAR *mode) {
  TCHAR tmpName[1024];
  TCHAR *sl;
  _tcscpy(tmpName, name);
  if(sl = _tcsrchr(tmpName, _T('\\'))) {
    *sl = 0;
    xmkdir(tmpName);
    *sl = _T('\\');
  }

  return FOPEN(tmpName, mode) ;
}

FILE *mkfopen(const String &name, const String &mode) {
  return mkfopen(name.cstr(), mode.cstr());
}

FILE *MKFOPEN(const TCHAR *name, const TCHAR *mode) {
  FILE *f = mkfopen(name, mode);
  if(f == NULL) {
    throwErrNoOnNameException(name);
  }
  return f;
}

FILE *MKFOPEN(const String &name, const String &mode) {
  return MKFOPEN(name.cstr(), mode.cstr());
}
