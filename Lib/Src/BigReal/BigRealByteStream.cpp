#include "pch.h"

#define NORMAL_FLAG   0x01
#define NEGATIVE_FLAG 0x02

void BigReal::save(ByteOutputStream &s) const {
  BYTE b = 0;
  if(!_isnormal()) {
    b = isNegative() ? NEGATIVE_FLAG : 0;
    s.putByte(b);
    const int cl = fpclassify(*this);
    s.putBytes((BYTE*)&cl, sizeof(cl));
  } else {
    b = NORMAL_FLAG | (isNegative() ? NEGATIVE_FLAG : 0);
    s.putByte(b);
    s.putBytes((BYTE*)&m_expo, sizeof(m_expo));
    CompactArray<BRDigitType> a(getLength());
    for(Digit *p = m_first; p; p = p->next) {
      a.add(p->n);
    }
    a.save(s);
  }
}

void BigReal::load(ByteInputStream &s) {
  const int b = s.getByte();
  if(b == EOF) {
    throwBigRealException(_T("Unexpected enf of stream"));
  }
  if((b & NORMAL_FLAG) == 0) {
    int cl;
    s.getBytesForced((BYTE*)&cl, sizeof(cl));
    switch(cl) {
    case FP_ZERO    :
      setToZero();
      break;
    case FP_NAN     :
      setToNan();
      break;
    case FP_INFINITE:
      setToInf();
      m_negative = (b & NEGATIVE_FLAG) ? true : false;
      break;
    }
  } else {
    clearDigits();
    m_negative = (b & NEGATIVE_FLAG) ? true : false;
    s.getBytesForced((BYTE*)&m_expo, sizeof(m_expo));
    CompactArray<BRDigitType> a;
    a.load(s);
    const size_t length = a.size();
    for(size_t i = 0; i < length; i++) {
      appendDigit(a[i]);
    }
    m_low = m_expo - length + 1;
  }
}