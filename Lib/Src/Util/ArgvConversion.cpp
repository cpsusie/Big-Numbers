#include "pch.h"
#include <comdef.h>
#include <atlconv.h>

template<class ftype, class ttype> ttype **argv2argv(const ftype **argv) {
  int count = 0;
  for(const ftype **tmp = argv; *tmp; tmp++) count++;
  ttype **result = new ttype*[count+1], **ttmp = result;

  USES_CONVERSION;
  for(const ftype **ftmp = argv; *ftmp; ftmp++, ttmp++) {
    *ttmp = (sizeof(ftype)>sizeof(ttype))?((ttype*)_strdup(W2A(*(wchar_t**)ftmp))):((ttype*)_wcsdup(A2W(*(char**)ftmp)));
  }
  *ttmp = NULL;
  return result;
};

wchar_t **argv2wargv(const char **argv) {
  return argv2argv<char,wchar_t>(argv);
}

char **wargv2argv(const wchar_t **targv) {
  return argv2argv<wchar_t,char>(targv);
}
