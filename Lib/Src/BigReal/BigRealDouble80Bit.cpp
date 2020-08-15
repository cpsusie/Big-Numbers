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
    const ConstBigReal &p2 = pow2(expo2, CONVERSION_POW2DIGITCOUNT);
    shortProductNoZeroCheck(significand, p2, 5).rRound(22);
  }
  if(x.isNegative()) {
    flipSign();
  }
}

bool isDouble80(const BigReal &v, Double80 *d80 /*=NULL*/) {
  if(!isnormal(v)) {
    if(d80) {
      *d80 = (Double80)v;
    }
    return true;
  }
  if((BigReal::compareAbs(v, BigReal::_dbl80_max) > 0) || (BigReal::compareAbs(v, BigReal::_dbl80_min) < 0) || (v.getDecimalDigits() > DBL80_DIG)) {
    return false;
  }
  const Double80 d   = v.getDouble80NoLimitCheck();
  const bool     ret = v == ConstBigReal(d);
  if(ret && d80) {
    *d80 = d;
  }
  return ret;
}

BigReal::operator Double80() const {
  DEFINEMETHODNAME;
  if(!_isnormal()) {
    return getNonNormalValue(_fpclass(*this), Double80::_0);
  }
/*
  if(validate) {
    if(BigReal::compareAbs(x,BigReal::_dbl80_max) > 0) {
      throwBigRealGetIntegralTypeOverflowException(method, x, toString(BigReal::_dbl80_max));
    }
    if(BigReal::compareAbs(x,BigReal::_dbl80_min) < 0) {
      throwBigRealGetIntegralTypeUnderflowException(method, x, toString(BigReal::_dbl80_min));
    }
  }
*/
  return getDouble80NoLimitCheck();
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
  xi.clrInitDone();
  if(expo2 <= minExpo2) {
    e2 = Double80::pow2(minExpo2);
    e2Overflow = false;
    xi.shortProductNoZeroCheck(::cut(*this,21), pow2(minExpo2, CONVERSION_POW2DIGITCOUNT), 10);  // BigReal multiplication
  } else if(expo2 >= maxExpo2) {
    e2  = Double80::pow2(maxExpo2);
    e2x = Double80::pow2((int)expo2 - maxExpo2);
    e2Overflow = true;
    xi.shortProductNoZeroCheck(::cut(*this,21), pow2((int)expo2, CONVERSION_POW2DIGITCOUNT), 10);  // BigReal multiplication
  } else {
    e2 = Double80::pow2((int)expo2);
    e2Overflow = false;
    xi = round(xi.shortProductNoNormalCheck(::cut(*this,22), pow2((int)expo2, CONVERSION_POW2DIGITCOUNT), -1));  // BigReal multiplication
  }
  xi.setInitDone();
  const Digit *p = xi.m_first;
  if(p == NULL) {
    return Double80::_0;
  }
  Double80 result     = (INT64)p->n;
  int      digitCount = BIGREAL_LOG10BASE;
  for(p = p->next; p && (digitCount < 24); p = p->next, digitCount += BIGREAL_LOG10BASE) {
    result *= BIGREALBASE;
    result += (INT64)p->n;
  }
  const BRExpoType e = xi.m_expo * BIGREAL_LOG10BASE - digitCount + BIGREAL_LOG10BASE;
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
