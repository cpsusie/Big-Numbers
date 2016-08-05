#include "pch.h"
#include <ctype.h>

#ifndef FAST_BIGREAL_CONVERSION

#define getSignificandDouble(x) (((*((unsigned __int64*)(&(x)))) & 0xfffffffffffffui64) | 0x10000000000000ui64)
#define getExpo2Double(x)       ((int)((((*((unsigned __int64*)(&(x)))) >> 52) & 0x7ff) - 0x3ff))

#define SIGNIFICANT_EXPO10 15


void BigReal::init(double x) {
  init();

  if(x != 0) {
    const int expo2 = getExpo2Double(x) - 52;
    if(expo2 == 0) {
      init(getSignificandDouble(x));
    } else {
      DigitPool *pool = getDigitPool();
      const BigReal significand(getSignificandDouble(x), pool);
      const bool isConstPool = pool->getId() == CONST_DIGITPOOL_ID;
      if (isConstPool) ConstDigitPool::releaseInstance(); // unlock it or we will get a deadlock
      const BigReal &p2 = pow2(expo2,CONVERSION_POW2DIGITCOUNT);
      if (isConstPool) ConstDigitPool::requestInstance();

//      const int     fexpo = (getExpo10(p2) + SIGNIFICANT_EXPO10 - CONVERSION_POW2DIGITCOUNT) / LOG10_BIGREALBASE;

      shortProductNoZeroCheck(significand, p2, 4).rRound(17);
/*
      const BigReal significand(getSignificandDouble(x), getDigitPool());
      const BigReal &p2 = pow2(expo2,CONVERSION_POW2DIGITCOUNT);
      const int     fexpo = (getExpo10(p2) + SIGNIFICANT_EXPO10 - CONVERSION_POW2DIGITCOUNT) / LOG10_BIGREALBASE;

      shortProduct(significand, p2, fexpo).rRound(17);
*/
    }
    if(x < 0) {
      m_negative = true;
    }
  }
}

double getDouble(const BigReal &x) {
  DEFINEMETHODNAME;

  if(x.isZero()) {
    return 0;
  }

  if(compareAbs(x,ConstBigReal::_dbl_max) > 0) {
    throwBigRealGetIntegralTypeOverflowException(method, x, toString(ConstBigReal::_dbl_max));
  }
  if(compareAbs(x,ConstBigReal::_dbl_min) < 0) {
    throwBigRealGetIntegralTypeUnderflowException(method, x, toString(ConstBigReal::_dbl_min));
  }

  return getDouble(x.getDouble80NoLimitCheck());
}

#else

// Faster than the code above, but has bad precision. Relative error about 1e-16

BigReal::BigReal(double x) {
  init();

  if(x != 0) {
    if(x < 0) {
      m_negative = true;
      x = -x;
    }
    m_expo = logBASE(x);
    int expo10 = ::getExpo10(x);
    x /= pow(10,expo10);
    x /= 10;
    if(x < 0.1) {
      if(x < 0.09) {
        expo10--;
      }
      x = 0.1;
    } else if(x >= 1) {
      expo10++;
      x = 0.1;
    }

    // assume 0.1 <= x < 1
    m_low = m_expo + 1;
    for(int i = 0; i < DBL_DIG && x != 0; i += LOG10_BIGREALBASE) {
      volatile double y = x * BIGREALBASE;
      x = y;
      int missingDecimals = DBL_DIG - i;
      if(missingDecimals < LOG10_BIGREALBASE) {
        appendDigit((unsigned long)round(y,LOG10_BIGREALBASE-missingDecimals));
      } else {
        appendDigit((unsigned long)y);
      }
      x = fraction(y);
      m_low--;
    }
    trimZeroes();
    multPow10(expo10 - getExpo10(*this));
  }
}

double getDouble(const BigReal &x) {
  static const char *method = "getDouble";

  if(x.isZero()) {
    return 0;
  }

  if(compareAbs(x,ConstBigReal::_dbl_max) > 0) {
    throwBigRealGetIntegralTypeOverflowException(method, x, toString(ConstBigReal::_dbl_max));
  }
  if(compareAbs(x,ConstBigReal::_dbl_min) < 0) {
    throwBigRealGetIntegralTypeUnderflowException(method, x, toString(ConstBigReal::_dbl_min));
  }

  const Digit *p = x.m_first;
  const int firstDigitCount = BigReal::getDecimalDigitCount(p->n);
  int digitCount      = firstDigitCount;
  double result = p->n;
  for(p = p->next; p && digitCount < DBL_DIG; p = p->next) {
    int needed = DBL_DIG - digitCount + 1;
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
    return x.m_negative ? -result : result;
  } else {
    if(scale10 > DBL_MAX_10_EXP) {        // Prevent overflow
      result = pow(10,x.m_expo * LOG10_BIGREALBASE - digitCount) * (x.m_negative ? -result : result);
      return result * BigReal::pow10(firstDigitCount);
    } else if(scale10 < DBL_MIN_10_EXP) { // Prevent underflow
      result = pow(10,x.m_expo * LOG10_BIGREALBASE + firstDigitCount) * (x.m_negative ? -result : result);
      return result / pow(10,digitCount); 
    } else {
      return pow(10,scale10) * (x.m_negative ? -result : result);
    }
  }
}

#endif
