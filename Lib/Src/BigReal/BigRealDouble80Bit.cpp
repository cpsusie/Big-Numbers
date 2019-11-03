#include "pch.h"
#include <ctype.h>

void BigReal::init(const Double80 &x) {
  initToZero();
  if(!isnormal(x)) {
    setToNonNormalFpClass(_fpclass(x));
    return;
  }
  // x is normal and != 0
  const int expo2 = getExpo2(x) - 63;
  if(expo2 == 0) {
    init(getSignificand(x));
  } else {
    DigitPool *pool = getDigitPool();
    const BigReal significand(getSignificand(x), pool);
    const bool isConstPool = pool->getId() == CONST_DIGITPOOL_ID;
    if(isConstPool) ConstDigitPool::releaseInstance(); // unlock it or we will get a deadlock
    const BigReal &p2 = pow2(expo2, CONVERSION_POW2DIGITCOUNT);
    if(isConstPool) ConstDigitPool::requestInstance();
    shortProductNoZeroCheck(significand, p2, 5).rRound(22);
  }
  if(x.isNegative()) {
    m_negative = true;
  }
}

bool isDouble80(const BigReal &v, Double80 *d80 /*=NULL*/) {
  if(!isnormal(v)) {
    if(d80) {
      *d80 = getDouble80(v);
    }
    return true;
  }
  if((compareAbs(v, BigReal::_dbl80_max) > 0) || (compareAbs(v, BigReal::_dbl80_min) < 0) || (v.getDecimalDigits() > DBL80_DIG)) {
    return false;
  }
  const Double80 d   = v.getDouble80NoLimitCheck();
  const bool     ret = v.isConst() ? (BigReal(d) == v) : (BigReal(d, v.getDigitPool()) == v);
  if(ret) {
    if(d80) {
      *d80 = d;
    }
    return true;
  }
  return false;
}

Double80 getDouble80(const BigReal &x) {
  DEFINEMETHODNAME;
  if(!isnormal(x)) {
    return getNonNormalValue(_fpclass(x), Double80::_0);
  }
  if(compareAbs(x,BigReal::_dbl80_max) > 0) {
    throwBigRealGetIntegralTypeOverflowException(method, x, toString(BigReal::_dbl80_max));
  }
  if(compareAbs(x,BigReal::_dbl80_min) < 0) {
    throwBigRealGetIntegralTypeUnderflowException(method, x, toString(BigReal::_dbl80_min));
  }
  return x.getDouble80NoLimitCheck();
}

Double80 BigReal::getDouble80NoLimitCheck() const {
  assert(_isnormal());
  static const int minExpo2 = 64-16382;
  static const int maxExpo2 = 0x3fff;

  DigitPool       *pool  = getDigitPool();
  const BRExpoType ee2   = getExpo2(*this);
  const BRExpoType expo2 = 64 - ee2;
  bool             e2Overflow;
  Double80         e2, e2x;
  BigReal          xi(pool);
  if(expo2 <= minExpo2) {
    e2 = Double80::pow2(minExpo2);
    e2Overflow = false;
    xi.shortProductNoNormalCheck(::cut(*this,21), pow2(minExpo2, CONVERSION_POW2DIGITCOUNT), BIGREAL_NONNORMAL);  // BigReal multiplication
  } else if(expo2 >= maxExpo2) {
    e2  = Double80::pow2(maxExpo2);
    e2x = Double80::pow2((int)expo2 - maxExpo2);
    e2Overflow = true;
    xi.shortProductNoNormalCheck(::cut(*this,21), pow2((int)expo2, CONVERSION_POW2DIGITCOUNT), BIGREAL_NONNORMAL);  // BigReal multiplication
  } else {
    e2 = Double80::pow2((int)expo2);
    e2Overflow = false;
    xi = round(xi.shortProductNoNormalCheck(::cut(*this,22), pow2((int)expo2, CONVERSION_POW2DIGITCOUNT), -1));  // BigReal multiplication
  }
  const Digit *p = xi.m_first;
  if(p == NULL) {
    return Double80::_0;
  }
  Double80 result     = (INT64)p->n;
  int      digitCount = LOG10_BIGREALBASE;
  for(p = p->next; p && (digitCount < 24); p = p->next, digitCount += LOG10_BIGREALBASE) {
    result *= BIGREALBASE;
    result += (INT64)p->n;
  }
  const BRExpoType e = xi.m_expo * LOG10_BIGREALBASE - digitCount + LOG10_BIGREALBASE;
  if(e > 0) {
    result *= Double80::pow10((int)e);
  } else if(e < 0) {
    result /= Double80::pow10(-(int)e);
  }
  result /= e2;
  if(e2Overflow) {
    result /= e2x;
  }
  return isNegative() ? -result : result;
}
