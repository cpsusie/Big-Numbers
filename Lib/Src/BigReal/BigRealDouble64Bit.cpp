#include "pch.h"
#include <ctype.h>

#define SIGNIFICANT_EXPO10 15

void BigReal::init(double x) {
  initToZero();
  if(!isnormal(x)) {
    setToNonNormalFpClass(_fpclass(x));
    return;
  }

  // x is normal and != 0
  const int expo2 = getExpo2(x) - 52;
  if(expo2 == 0) {
    init(getSignificand(x));
  } else {
    DigitPool *pool = getDigitPool();
    const BigReal significand(getSignificand(x), pool);
    const bool isConstPool = pool->getId() == CONST_DIGITPOOL_ID;
    if(isConstPool) ConstDigitPool::releaseInstance(); // unlock it or we will get a deadlock
    const BigReal &p2 = pow2(expo2,CONVERSION_POW2DIGITCOUNT);
    if(isConstPool) ConstDigitPool::requestInstance();
    shortProductNoZeroCheck(significand, p2, 4).rRound(17);
  }
  if(x < 0) {
    m_negative = true;
  }
}

bool isDouble(const BigReal &v, double *dbl /*=NULL*/) {
  if(!isnormal(v)) {
    if(dbl) {
      *dbl = getDouble(v);
    }
    return true;
  }
  if((compareAbs(v, BigReal::_dbl_max) > 0) || (compareAbs(v, BigReal::_dbl_min) < 0) || (v.getDecimalDigits() > DBL_DIG)) {
    return false;
  }
  const double d   = v.getDoubleNoLimitCheck();
  const bool   ret = v.isConst() ? (ConstBigReal(d) == v) : (BigReal(d, v.getDigitPool()) == v);
  if(ret) {
    if(dbl) {
      *dbl = d;
    }
    return true;
  }
  return false;
}

double getDouble(const BigReal &x) {
  DEFINEMETHODNAME;
  if(!isnormal(x)) {
    return getNonNormalValue(_fpclass(x), 0.0);
  }
  if(compareAbs(x,BigReal::_dbl_max) > 0) {
    throwBigRealGetIntegralTypeOverflowException(method, x, toString(BigReal::_dbl_max));
  }
  if(compareAbs(x,BigReal::_dbl_min) < 0) {
    throwBigRealGetIntegralTypeUnderflowException(method, x, toString(BigReal::_dbl_min));
  }
  return x.getDoubleNoLimitCheck();
}
