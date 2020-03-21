#include "pch.h"
#include <Math/BigReal/BigInt.h>

BigInt::BigInt(const BigReal &x, DigitPool *digitPool) : BigReal(digitPool ? digitPool : x.getDigitPool()) {
  digitPool = getDigitPool();
  clrInitDone();
  if(!x._isnormal()) {
    setToNonNormal(x.m_low, x.isNegative());
  } else if(x.m_expo < 0) { // |x| < 1
    *this = x.isNegative() ? -digitPool->_1() : digitPool->_0();
  } else if(x.getLow() >= 0) { // x is an integer
    copyAllDigits(x);
    copyNonPointerFields(x);
  } else { // |x| > 1 and x is not an integer
    copyDigits(x, (m_expo = x.m_expo)+1);
    m_low      = 0;
    if(copySign(x).trimZeroes().isNegative()) {
      --(*this);
    }
  }
  setInitDone();
}

BigInt::BigInt(const BigInt &x, DigitPool *digitPool) : BigReal(digitPool ? digitPool : x.getDigitPool()) {
  clrInitDone();
  if(!x._isnormal()) {
    setToNonNormal(x.m_low, x.isNegative());
  } else {
    copyAllDigits(x);
    copyNonPointerFields(x);
  }
  setInitDone();
}

BigInt::BigInt(const String &s, DigitPool *digitPool) : BigReal(digitPool) {
  clrInitDone();
  init(s, false);
  setInitDone();
}

BigInt::BigInt(const char *s, DigitPool *digitPool) : BigReal(digitPool) {
  clrInitDone();
  init(s, false);
  setInitDone();
}

BigInt::BigInt(const wchar_t *s, DigitPool *digitPool) : BigReal(digitPool) {
  clrInitDone();
  init(s, false);
  setInitDone();
}

BigInt quot(const BigInt &x, const BigInt &y, DigitPool *digitPool) {
  _SELECTDIGITPOOL(x);
  BigInt result(pool);
  result.clrInitDone();
  quotRemainder(x, y, &result, NULL);
  result.setSignByProductRule(x, y).setInitDone();
  return result;
}

BigInt rem(const BigInt &x, const BigInt &y, DigitPool *digitPool) {
  _SELECTDIGITPOOL(x);
  BigInt result(pool);
  result.clrInitDone();
  quotRemainder(x, y, NULL, &result);
  result.setInitDone();
  return result;
}

#define _BR2 (BIGREALBASE / 2)
BigInt &BigInt::divide2() {
  CHECKISMUTABLE(*this);
  bool borrow = false;
  if(_isnormal()) {
    for(Digit *d = m_first; d; d = d->next) {
      const bool nb = (d->n & 1) != 0;
      d->n /= 2;
      if(borrow) d->n += _BR2;
      borrow = nb;
    }
    if(borrow && (m_low > 0)) {
      appendDigit(_BR2);
    } else {
      trimZeroes();
    }
  }
  return *this;
}
