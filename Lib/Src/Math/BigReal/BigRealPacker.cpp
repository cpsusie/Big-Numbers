#include "pch.h"
#include <tcp.h>

#define ZERO_FLAG     0x01
#define NEGATIVE_FLAG 0x02

Packer &operator<<(Packer &p, const BigReal &n) {
  BYTE b = 0;
  if(n.isZero()) {
    b = ZERO_FLAG;
  } else if(n.isNegative()) {
    b = NEGATIVE_FLAG;
  }
  p << b;
  if(!n.isZero()) {
    p << n.m_expo;
    const int length = n.getLength();
    CompactLongArray digits(length);
    p << length;
    for(const Digit *d = n.m_first; d; d = d->next) {
      digits.add(htonl(d->n));
    }
    p.addElement(Packer::E_LONG, digits.getBuffer(), length * sizeof(unsigned long));
  }
  return p;
}

Packer &operator>>(Packer &p, BigReal &n) {
  BYTE b;
  p >> b;
  if(b & ZERO_FLAG) {
    n.setToZero();
  } else {
    n.clearDigits();
    n.m_negative = (b & NEGATIVE_FLAG) ? true : false;
    p >> n.m_expo;
    int length;
    p >> length;
    unsigned long *da = NULL;
    try { 
      da = new unsigned long[length];
      p.getElement(Packer::E_LONG, da, length * sizeof(unsigned long));
      unsigned long *d = da;
      for(int i = length; i--;) {
        n.appendDigit(ntohl(*(d++)));
      }
      n.m_low = n.m_expo - length + 1;
      delete[] da;
    } catch(...) {
      if(da) delete[] da;
      throw;
    }
  }
  SETBIGREALDEBUGSTRING(n);
  return p;
}
