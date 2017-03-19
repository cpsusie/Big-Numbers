#include "pch.h"
#include "Packer.h"

#ifdef UNICODE
#define E_STRING E_WSTR
#else
#define E_STRING E_STR
#endif

Packer &Packer::operator<<(const String &s) {
  const size_t l = s.length();
  *this << l;
  addElement(E_STRING,s.cstr(),l*sizeof(TCHAR));
  return *this;
}

Packer &Packer::operator>>(String &s) {
  size_t l;
  *this >> l;
  switch(peekType()) {
  case E_CSTR  :
    { char *tmp = new char[l+1];
      try {
        getElement(E_CSTR, tmp, l);
        tmp[l] = 0;
        s = tmp;
        delete[] tmp;
      } catch (...) {
        delete[] tmp;
        throw;
      }
    }
    break;
  case E_WSTR :
    { wchar_t *tmp = new wchar_t[l+1];
      try {
        getElement(E_WSTR, tmp, l*sizeof(wchar_t));
        tmp[l] = 0;
        s = tmp;
        delete[] tmp;
      } catch (...) {
        delete[] tmp;
        throw;
      }
    }
    break;
  default:
    throwException(_T("%s:Invalid type:%d. Expected E_STR/E_WSTR"), __TFUNCTION__, peekType());
  }
  return *this;
}

