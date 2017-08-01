#include "pch.h"
#include "Packer.h"

Packer &Packer::operator<<(const ByteArray &a) {
  const size_t l = a.size();
  *this << l;
  if(l) {
    addElement(E_CHAR, a.getData(), l);
  }
  return *this;
}

Packer &Packer::operator>>(ByteArray &a) {
  size_t l;
  *this >> l;
  a.clear();
  if(l) {
    BYTE *tmp = new BYTE[l]; TRACE_NEW(tmp);
    getElement(E_CHAR, tmp, l);
    a.setData(tmp, l);
    SAFEDELETEARRAY(tmp);
  }
  return *this;
}
