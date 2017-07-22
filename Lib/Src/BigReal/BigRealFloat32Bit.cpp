#include "pch.h"
#include <ctype.h>

#ifndef FAST_BIGREAL_CONVERSION

#define getSignificandFloat(x) ((UINT)(((*((unsigned long*)(&(x)))) & 0x7fffff) | 0x800000))
#define getExpo2Float(x)       ((int)((((*((unsigned long*)(&(x)))) >> 23) & 0xff) - 0x7f))

void BigReal::init(float x) {
  init();

  if(x != 0) {
    const int expo2 = getExpo2Float(x) - 23;
    if(expo2 == 0) {
      init(getSignificandFloat(x));
    } else {
      DigitPool *pool = getDigitPool();
      const BigReal significand(getSignificandFloat(x), pool);
      const bool isConstPool = pool->getId() == CONST_DIGITPOOL_ID;
      if (isConstPool) ConstDigitPool::releaseInstance(); // unlock it or we will get a deadlock
      const BigReal &p2 = pow2(expo2, CONVERSION_POW2DIGITCOUNT);
      if (isConstPool) ConstDigitPool::requestInstance();
      shortProductNoZeroCheck(significand, p2, 2).rRound(9);
/*
      const BigReal p2 = ::cut(pow2(expo2, CONVERSION_POW2DIGITCOUNT), 9, getDigitPool());
      const int fexpo = (getExpo10(p2) + getExpo10(significand) - CONVERSION_POW2DIGITCOUNT) / LOG10_BIGREALBASE;

      shortProduct(significand, p2, fexpo).rRound(9);
*/
    }
    if(x < 0) {
      m_negative = true;
    }
  }
}

float getFloat(const BigReal &x) {
  DEFINEMETHODNAME;
  if(x.isZero()) {
    return 0;
  }

  if(compareAbs(x,ConstBigReal::_flt_max) > 0) {
    throwBigRealGetIntegralTypeOverflowException(method, x, toString(ConstBigReal::_flt_max));
  }
  if(compareAbs(x,ConstBigReal::_flt_min) < 0) {
    throwBigRealGetIntegralTypeUnderflowException(method, x, toString(ConstBigReal::_flt_min));
  }

  return getFloat(x.getDouble80NoLimitCheck());
}

#else

// Faster than the code above, but has bad precision. Relative error about 1e-6

BigReal::BigReal(float x) {
  init();

  if(x != 0) {
    if(x < 0) {
      m_negative = true;
      x = -x;
    }
    m_expo = logBASE(x);
    int expo10 = ::getExpo10(x);
    x /= pow(10,expo10+1);
    if(x < 0.1) {
      if(x < 0.09) {
        expo10--;
      }
      x = 0.1f;
    } else if(x >= 1) {
      expo10++;
      x = 0.1f;
    }

    // assume 0.1 <= x < 1
    m_low = m_expo + 1;
    for(int i = 0; i < FLT_DIG && x != 0; i += LOG10_BIGREALBASE) {
      volatile float  y = x * BIGREALBASE;
      x = y;
      int missingDecimals = FLT_DIG - i;
      if(missingDecimals < LOG10_BIGREALBASE) {
        appendDigit((unsigned long)round(y,LOG10_BIGREALBASE-missingDecimals));
      } else {
        appendDigit((unsigned long)y);
      }
      x = fraction(x);
      m_low--;
    }
    trimZeroes();
    multPow10(expo10 - getExpo10(*this));
  }
}

float getFloat(const BigReal &x) {
  DEFINEMETHODNAME;

  if(x.isZero()) {
    return 0;
  }

  if(compareAbs(x,ConstBigReal::_flt_max) > 0) {
    throwBigRealGetIntegralTypeOverflowException(method, x, toString(ConstBigReal::_flt_max));
  }
  if(compareAbs(x,ConstBigReal::_flt_min) < 0) {
    throwBigRealGetIntegralTypeUnderflowException(method, x, toString(ConstBigReal::_flt_min));
  }

  const Digit *p = x.m_first;
  const int firstDigitCount = BigReal::getDecimalDigitCount(p->n);
  int digitCount      = firstDigitCount;
  double result = p->n;
  for(p = p->next; p && digitCount < FLT_DIG; p = p->next) {
    int needed = FLT_DIG - digitCount + 1;
    if(needed >= LOG10_BIGREALBASE) {
      result = result * BIGREALBASE + p->n;
      digitCount += LOG10_BIGREALBASE;
    } else {
      int n10 = BigReal::pow10(needed);
      result = result * n10 + p->n / (BIGREALBASE/n10);
      digitCount += needed;
    }
  }

  const int scale10 = x.m_expo * LOG10_BIGREALBASE - digitCount + firstDigitCount;
  if(scale10 == 0) {
    return (float)(x.m_negative ? -result : result);
  } else {
    if(scale10 > FLT_MAX_10_EXP) {        // Prevent overflow
      result = pow(10,x.m_expo * LOG10_BIGREALBASE - digitCount) * (x.m_negative ? -result : result);
      return (float)(result * BigReal::pow10(firstDigitCount));
    } else if(scale10 < FLT_MIN_10_EXP) { // Prevent underflow
      result = pow(10,x.m_expo * LOG10_BIGREALBASE + firstDigitCount) * (x.m_negative ? -result : result);
      return (float)(result / pow(10,digitCount));
    } else {
      return (float)(pow(10,scale10) * (x.m_negative ? -result : result));
    }
  }
}

#endif
