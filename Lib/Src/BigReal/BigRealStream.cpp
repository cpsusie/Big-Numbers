#include "pch.h"

#define ZERO_FLAG     0x01
#define NEGATIVE_FLAG 0x02

void BigReal::save(ByteOutputStream &s) const {
  BYTE b = 0;
  if(isZero()) {
    b = ZERO_FLAG;
  } else if(isNegative()) {
    b = NEGATIVE_FLAG;
  }
  s.putByte(b);
  if(!isZero()) {
    s.putBytes((BYTE*)&m_expo, sizeof(m_expo));
    const int length = getLength();
    s.putBytes((BYTE*)&length, sizeof(length));
    for(Digit *p = m_first; p; p = p->next) {
      s.putBytes((BYTE*)&(p->n), sizeof(p->n));
    }
  }
}

void BigReal::load(ByteInputStream &s) {
  int b = s.getByte();
  if(b == EOF) {
    throwBigRealException(_T("Unexpected enf of stream"));
  }
  if(b & ZERO_FLAG) {
    setToZero();
  } else {
    clearDigits();
    m_negative = (b & NEGATIVE_FLAG) ? true : false;
    s.getBytesForced((BYTE*)&m_expo, sizeof(m_expo));
    int length;
    s.getBytesForced((BYTE*)&length, sizeof(length));
    for(int i = 0; i < length; i++) {
      unsigned long n;
      s.getBytesForced((BYTE*)(&n), sizeof(n));
      assert(n < BIGREALBASE);
      appendDigit(n);
    }
    m_low = m_expo - length + 1;
  }
  SETBIGREALDEBUGSTRING(*this);
}
