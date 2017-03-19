#include "pch.h"
#include "Packer.h"
#include <comdef.h>
#include <atlconv.h>

Packer &Packer::operator<<(const wchar_t * const s) {
  const size_t l = wcslen(s);
  *this << l;
  addElement(E_WSTR,s,l*sizeof(wchar_t));
  return *this;
}

Packer &Packer::operator>>(wchar_t *s) {
  USES_CONVERSION;
  size_t l;
  *this >> l;
  switch(peekType()) {
  case E_CSTR:
    { char *cstr = new char[l+1];
      try {
        getElement(E_CSTR,cstr,l*sizeof(char));
        cstr[l] = 0;
        wchar_t *wstr = A2W(cstr);
        _tcscpy(s, wstr);
        delete[] cstr;
      } catch(...) {
        delete[] cstr;
        throw;
      }
    }
    break;
  case E_WSTR:
    getElement(E_WSTR,s,l*sizeof(wchar_t));
    s[l] = 0;
    break;
  default:
    throwException(_T("%s:Invalid type:%d. Expected E_CSTR/E_WSTR"), __TFUNCTION__, peekType());
  }
  return *this;
}
