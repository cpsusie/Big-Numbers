#include "pch.h"
#include <ctype.h>

#ifndef FAST_BIGREAL_CONVERSION

void BigReal::init(const Double80 &x) {
  DEFINEMETHODNAME;

  init();

  if (isNan(x)) {
    throwBigRealInvalidArgumentException(method, _T("Double80 is Nan"));
  }
  if(!x.isZero()) {
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
/*
      const BigReal significand(getSignificand(x), getDigitPool());
      BigReal p2(getDigitPool());
      copy(p2, pow2(expo2, CONVERSION_POW2DIGITCOUNT), 32 / LOG10_BIGREALBASE); // somewhere between 25 and 32 decimal digits
//      const BigReal p2 = cut(pow2(expo2, CONVERSION_POW2DIGITCOUNT), 21, getDigitPool());

      const int fexpo = (getExpo10(p2) + getExpo10(significand) - CONVERSION_POW2DIGITCOUNT) / LOG10_BIGREALBASE + 1;
      shortProduct(significand, p2, fexpo).rRound(22);
*/
    }
    if(x.isNegative()) {
      m_negative = true;
    }
  }
}

Double80 getDouble80(const BigReal &x) {
  DEFINEMETHODNAME;

  if(x.isZero()) {
    return Double80::zero;
  }
  if(compareAbs(x,ConstBigReal::_dbl80_max) > 0) {
    throwBigRealGetIntegralTypeOverflowException(method, x, toString(ConstBigReal::_dbl80_max));
  }
  if(compareAbs(x,ConstBigReal::_dbl80_min) < 0) {
    throwBigRealGetIntegralTypeUnderflowException(method, x, toString(ConstBigReal::_dbl80_min));
  }
  return x.getDouble80NoLimitCheck();
}

Double80 BigReal::getDouble80NoLimitCheck() const {
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
    xi.shortProduct(::cut(*this,21), pow2(minExpo2, CONVERSION_POW2DIGITCOUNT), BIGREAL_ZEROEXPO);  // BigReal multiplication
  } else if(expo2 >= maxExpo2) {
    e2  = Double80::pow2(maxExpo2);
    e2x = Double80::pow2((int)expo2 - maxExpo2);
    e2Overflow = true;
    xi.shortProduct(::cut(*this,21), pow2((int)expo2, CONVERSION_POW2DIGITCOUNT), BIGREAL_ZEROEXPO);  // BigReal multiplication
  } else {
    e2 = Double80::pow2((int)expo2);
    e2Overflow = false;
    xi = round(xi.shortProduct(::cut(*this,22), pow2((int)expo2, CONVERSION_POW2DIGITCOUNT), -1));  // BigReal multiplication
  }
  const Digit *p = xi.m_first;
  if(p == NULL) {
    return Double80::zero;
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

#else

// Faster than the code above, but has bad precision. Relative error about 1e-16

static int logBASE(const Double80 &x) {
  return getInt(log10(x) / LOG10_BIGREALBASE);
}

BigReal::BigReal(const Double80 &x) {
  static const Double80 base = BIGREALBASE;
  static const Double80 c1 = Double80::one/10;
  static const Double80 c2 = 0.09;
  static const Double80 c3 = 10;

  init();

  if(x != Double80::zero) {
    Double80 tmp;

    if(x < Double80::zero) {
      m_negative = true;
      tmp = -x;
    } else {
      tmp = x;
    }
    m_expo = ::logBASE(tmp);
    int expo10 = Double80::getExpo10(tmp);
    if(expo10 > 4000) {
      tmp /= Double80::pow10(expo10);
      tmp /= c3;
    } else {
      tmp /= Double80::pow10(expo10+1);
    }

    if(tmp < c1) {
      if(tmp < c2) {
        expo10--;
      }
      tmp = c1;
    } else if(tmp >= Double80::one) {
      expo10++;
      tmp = c1;
    }

    // assume 0.1 <= tmp < 1
    m_low = m_expo + 1;
    for(int i = 0; i < DBL80_DIG && tmp != Double80::zero; i += LOG10_BIGREALBASE) {
      tmp *= base;
      int missingDecimals = DBL80_DIG - i;
      if(missingDecimals < LOG10_BIGREALBASE) {
        appendDigit(getUlong(round(tmp,missingDecimals-LOG10_BIGREALBASE+1)) % BIGREALBASE);
      } else {
        appendDigit(getUlong(tmp));
      }
      tmp = fraction(tmp);
      m_low--;
    }
    trimZeroes();
    multPow10(expo10 - getExpo10(*this));
  }
}

Double80 getDouble80(const BigReal &x) {
  DEFINEMETHODNAME;

  if(x.isZero()) {
    return Double80::zero;
  }

  if(compareAbs(x,ConstBigReal::_dbl80_max) > 0) {
    throwBigRealGetIntegralTypeOverflowException(method, x, toString(ConstBigReal::_dbl80_max));
  }
  if(compareAbs(x,ConstBigReal::_dbl80_min) < 0) {
    throwBigRealGetIntegralTypeUnderflowException(method, x, toString(ConstBigReal::_dbl80_min));
  }

  const Digit *p = x.m_first;
  const int firstDigitCount = BigReal::getDecimalDigitCount(p->n);
  int digitCount      = firstDigitCount;
  Double80 result = p->n;
  for(p = p->next; p && digitCount < DBL80_DIG; p = p->next) {
    int needed = DBL80_DIG - digitCount + 1;
    if(needed >= LOG10_BIGREALBASE) {
      result = result * BIGREALBASE + p->n;
      digitCount += LOG10_BIGREALBASE;
    } else {
      int n10 = BigReal::pow10(needed);
      result = result * n10 + p->n / (BIGREALBASE/n10);
      digitCount += needed;
    }
  }

  int scale10 = x.m_expo * LOG10_BIGREALBASE - digitCount + firstDigitCount;
  if(scale10 == 0) {
    return x.m_negative ? -result : result;
  } else {
    if(scale10 > 4900) {         // Prevent overflow
      result *= Double80::pow10(4900);
      scale10 -= 4900;
    } else if(scale10 < -4900) { // Prevent underflow
      result /= Double80::pow10(4900);
      scale10 += 4900;
    }
    return Double80::pow10(scale10) * (x.m_negative ? -result : result);
  }
}

#endif
