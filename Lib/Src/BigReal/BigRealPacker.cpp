#include "pch.h"
#include <tcp.h>

#define ZERO_FLAG     0x01
#define NEGATIVE_FLAG 0x02

typedef CompactArray<BRDigitType> CompactDigitArray;
#ifdef IS32BIT
#define BRPACKERTYPE Packer::E_LONG
#define htonDigit    htonl
#define ntohDigit    ntohl
#else
#define BRPACKERTYPE Packer::E_LONG_LONG
#define htonDigit    htonll
#define ntohDigit    ntohll
#endif

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
    const size_t length = n.getLength();
    CompactDigitArray digits(length);
    p << length;
    for(const Digit *d = n.m_first; d; d = d->next) {
      digits.add(htonDigit(d->n));
    }
    p.addElement(BRPACKERTYPE, digits.getBuffer(), length * sizeof(BRDigitType));
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
    size_t length;
    p >> length;
    BRDigitType *da = NULL;
    try { 
      da = new BRDigitType[length];
      p.getElement(Packer::BRPACKERTYPE, da, length * sizeof(BRDigitType));
      BRDigitType *d = da;
      for(size_t i = length; i--;) {
        n.appendDigit(ntohDigit(*(d++)));
      }
      n.m_low = n.m_expo - length + 1;
      delete[] da;
    } catch(...) {
      if(da) delete[] da;
      throw;
    }
  }
  return p;
}
