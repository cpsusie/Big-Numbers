#include "pch.h"
#include "Packer.h"

Packer &Packer::operator<<(const wchar_t * const s) {
  const size_t l = wcslen(s);
  *this << l;
  addElement(E_WSTR,s,l*sizeof(wchar_t));
  return *this;
}

Packer &Packer::operator>>(wchar_t *s) {
  size_t l;
  *this >> l;
  getElement(E_WSTR,s,l*sizeof(wchar_t));
  s[l] = 0;
  return *this;
}
