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
    const BigReal &p2 = pow2(expo2,CONVERSION_POW2DIGITCOUNT);
    shortProductNoZeroCheck(significand, p2, 4).rRound(17);
  }
  if(x < 0) {
    setNegative(true);
  }
}

bool isDouble(const BigReal &v, double *dbl /*=NULL*/) {
  if(!isnormal(v)) {
    if(dbl) {
      *dbl = getDouble(v);
    }
    return true;
  }
  if((BigReal::compareAbs(v, BigReal::_dbl_max) > 0) || (BigReal::compareAbs(v, BigReal::_dbl_min) < 0) || (v.getDecimalDigits() > DBL_DIG)) {
    return false;
  }
  const double d   = v.getDoubleNoLimitCheck();
  const bool   ret = v == ConstBigReal(d);
  if(ret && dbl) {
    *dbl = d;
  }
  return ret;
}

double getDouble(const BigReal &x) {
  DEFINEMETHODNAME;
  if(!isnormal(x)) {
    return getNonNormalValue(_fpclass(x), 0.0);
  }
  if(BigReal::compareAbs(x,BigReal::_dbl_max) > 0) {
    throwBigRealGetIntegralTypeOverflowException(method, x, toString(BigReal::_dbl_max));
  }
  if(BigReal::compareAbs(x,BigReal::_dbl_min) < 0) {
    throwBigRealGetIntegralTypeUnderflowException(method, x, toString(BigReal::_dbl_min));
  }
  return x.getDoubleNoLimitCheck();
}
