#include "pch.h"
#include <Math/BigInt.h>

BigInt::BigInt(const BigReal &x, DigitPool *digitPool) : BigReal(digitPool ? digitPool : x.getDigitPool()) {
  digitPool = getDigitPool();

  if(!x._isnormal()) {
    setToNonNormal(x.m_low, x.m_negative);
  } else if(x.m_expo < 0) { // |x| < 1
    *this = x.isNegative() ? -digitPool->_1() : digitPool->_0();
  } else if(x.getLow() >= 0) { // x is an integer
    copyAllDigits(x);
    copyNonPointerFields(x);
  } else { // |x| > 1 and x is not an integer
    copyDigits(x, (m_expo = x.m_expo)+1);
    m_low      = 0;
    m_negative = x.m_negative;
    trimZeroes();
    if(m_negative) {
      --(*this);
    }
  }
}

BigInt::BigInt(const BigInt &x, DigitPool *digitPool) : BigReal(digitPool ? digitPool : x.getDigitPool()) {
  if(!x._isnormal()) {
    setToNonNormal(x.m_low, x.m_negative);
  } else {
    copyAllDigits(x);
    copyNonPointerFields(x);
  }
}

BigInt::BigInt(const String &s, DigitPool *digitPool) : BigReal(digitPool) {
  init(s, false);
}

BigInt::BigInt(const char *s, DigitPool *digitPool) : BigReal(digitPool) {
  init(s, false);
}

BigInt::BigInt(const wchar_t *s, DigitPool *digitPool) : BigReal(digitPool) {
  init(s, false);
}

BigInt quot(const BigInt &x, const BigInt &y, DigitPool *digitPool) {
  if(digitPool == NULL) digitPool = x.getDigitPool();
  BigInt result(digitPool);
  quotRemainder(x, y, &result, NULL);
  result.setSignByProductRule(x, y);
  return result;
}

BigInt rem(const BigInt &x, const BigInt &y, DigitPool *digitPool) {
  if(digitPool == NULL) digitPool = x.getDigitPool();
  BigInt result(digitPool);
  quotRemainder(x, y, NULL, &result);
  return result;
}

#define _BR2 (BIGREALBASE / 2)
// fast version of *this /= 2
BigInt &BigInt::divide2() {
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

BigInt &BigInt::multiply2() {
  bool carry = false;
  if(_isnormal()) {
    for(Digit *d = m_last; d; d = d->prev) {
      d->n *= 2;
      if(carry) d->n++;
      if(carry = (d->n >= BIGREALBASE)) {
        d->n -= BIGREALBASE;
      }
    }
    if(carry) {
      insertDigit(1);
      m_expo++;
    }
    trimZeroes();
  }
  return *this;
}
