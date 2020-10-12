#include "pch.h"
#include <tchar.h>
#include <comdef.h>
#include <atlconv.h>

static char *strdupw2a(_In_z_ wchar_t const *s) {
  USES_CONVERSION;
  char *tmp = W2A(s);
  return _strdup(tmp);
}

static wchar_t *strdupa2w(_In_z_ char const *s) {
  USES_CONVERSION;
  wchar_t *tmp = A2W(s);
  return _wcsdup(tmp);
}

template<typename ftype, typename ttype> const ttype **argv2argv(const ftype **argv) {
  size_t count = 0;
  for(const ftype **tmp = argv; *tmp; tmp++) count++;
  ttype **result = new ttype*[(size_t)count+1], **ttmp = result;

  for(const ftype **ftmp = argv; *ftmp; ftmp++, ttmp++) {
    *ttmp = (sizeof(ftype)>sizeof(ttype))?((ttype*)strdupw2a(*(wchar_t**)ftmp)):((ttype*)strdupa2w(*(char**)ftmp));
  }
  *ttmp = nullptr;
  return (const ttype**)result;
};

const wchar_t **argv2wargv(const char **argv) {
  return argv2argv<char,wchar_t>(argv);
}

const char **wargv2argv(const wchar_t **argv) {
  return argv2argv<wchar_t,char>(argv);
}

const TCHAR **argv2targv(const char **argv) {
#if !defined(UNICODE)
  return (const TCHAR**)argv;
#else
  return argv2wargv(argv);
#endif
}

const TCHAR **argv2targv(const wchar_t **argv) {
#if defined(UNICODE)
  return argv;
#else
  return wargv2argv(argv);
#endif
}

const char **targv2argv(const TCHAR **argv) {
#if !defined(UNICODE)
  return (const TCHAR**)argv;
#else
  return wargv2argv(argv);
#endif
}

const wchar_t **targv2wargv(const TCHAR **argv) {
#if defined(UNICODE)
  return argv;
#else
  return wargv2argv(argv);
#endif
}
