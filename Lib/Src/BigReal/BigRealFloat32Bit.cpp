#include "pch.h"
#include <ctype.h>

void BigReal::init(float x) {
  initToZero();
  if(!isnormal(x)) {
    setToNonNormalFpClass(_fpclass(x));
    return;
  }

  // x is normal and != 0
  const int expo2 = getExpo2(x) - 23;
  if(expo2 == 0) {
    init(getSignificand(x));
  } else {
    DigitPool *pool = getDigitPool();
    const BigReal significand(getSignificand(x), pool);
    const BigReal &p2 = pow2(expo2, CONVERSION_POW2DIGITCOUNT);
    shortProductNoZeroCheck(significand, p2, 2).rRound(9);
  }
  if(x < 0) {
    flipSign();
  }
}

bool isFloat(const BigReal &v, float *flt /*=NULL*/) {
  if(!isnormal(v)) {
    if(flt) {
      *flt = (float)v;
    }
    return true;
  }
  if((BigReal::compareAbs(v, BigReal::_flt_max) > 0) || (BigReal::compareAbs(v, BigReal::_flt_min) < 0) || (v.getDecimalDigits() > FLT_DIG)) {
    return false;
  }
  const float f   = v.getFloatNoLimitCheck();
  const bool  ret = v == ConstBigReal(f);
  if(ret && flt) {
    *flt = f;
  }
  return ret;
}

BigReal::operator float() const {
  DEFINEMETHODNAME;

  if(!_isnormal()) {
    return getNonNormalValue(_fpclass(*this), 0.0f);
  }
/*
  if(validate) {
    if(BigReal::compareAbs(x,BigReal::_flt_max) > 0) {
      throwBigRealGetIntegralTypeOverflowException(method, x, toString(BigReal::_flt_max));
    }
    if(BigReal::compareAbs(x,BigReal::_flt_min) < 0) {
      throwBigRealGetIntegralTypeUnderflowException(method, x, toString(BigReal::_flt_min));
    }
  }
*/
  return getFloatNoLimitCheck();
}
