#include "pch.h"
#include <comdef.h>
#include <atlconv.h>

static char *strdupw2a(_In_z_ wchar_t const* s) {
  USES_CONVERSION;
  return _strdup(W2A(s));
}
static wchar_t *strdupa2w(_In_z_ char const* s) {
  USES_CONVERSION;
  return _wcsdup(A2W(s));
}

template<class ftype, class ttype> const ttype **argv2argv(const ftype **argv) {
  int count = 0;
  for(const ftype **tmp = argv; *tmp; tmp++) count++;
  ttype **result = new ttype*[count+1], **ttmp = result;

  for(const ftype **ftmp = argv; *ftmp; ftmp++, ttmp++) {
    *ttmp = (sizeof(ftype)>sizeof(ttype))?((ttype*)strdupw2a(*(wchar_t**)ftmp)):((ttype*)strdupa2w(*(char**)ftmp));
  }
  *ttmp = NULL;
  return (const ttype**)result;
};

const wchar_t **argv2wargv(const char **argv) {
  return argv2argv<char,wchar_t>(argv);
}

const char **wargv2argv(const wchar_t **targv) {
  return argv2argv<wchar_t,char>(targv);
}

const TCHAR **argv2targv(const char **argv) {
  if(sizeof(TCHAR) == sizeof(char)) {
    return (const TCHAR**)argv;
  } else {
    return argv2wargv(argv);
  }
}
