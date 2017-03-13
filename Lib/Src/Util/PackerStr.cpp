#include "pch.h"
#include "Packer.h"

Packer &Packer::operator<<(const char * const s) {
  const size_t l = strlen(s);
  *this << l;
  addElement(E_STR,s,l);
  return *this;
}

Packer &Packer::operator>>(char *s) {
  size_t l;
  *this >> l;
  getElement(E_STR,s,l);
  s[l] = 0;
  return *this;
}

