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
    const bool isConstPool = pool->getId() == CONST_DIGITPOOL_ID;
    if(isConstPool) ConstDigitPool::releaseInstance(); // unlock it or we will get a deadlock
    const BigReal &p2 = pow2(expo2, CONVERSION_POW2DIGITCOUNT);
    if(isConstPool) ConstDigitPool::requestInstance();
    shortProductNoZeroCheck(significand, p2, 2).rRound(9);
  }
  if(x < 0) {
    m_negative = true;
  }
}

bool isFloat(const BigReal &v, float *flt /*=NULL*/) {
  if(!isnormal(v)) {
    if(flt) {
      *flt = getFloat(v);
    }
    return true;
  }
  if((compareAbs(v, BigReal::_flt_max) > 0) || (compareAbs(v, BigReal::_flt_min) < 0) || (v.getDecimalDigits() > FLT_DIG)) {
    return false;
  }
  const float f   = v.getFloatNoLimitCheck();
  const bool  ret = v.isConst() ? (ConstBigReal(f) == v) : (BigReal(f, v.getDigitPool()) == v);
  if(ret) {
    if(flt) {
      *flt = f;
    }
    return true;
  }
  return false;
}

float getFloat(const BigReal &x) {
  DEFINEMETHODNAME;

  if(!isnormal(x)) {
    return getNonNormalValue(_fpclass(x), 0.0f);
  }
  if(compareAbs(x,BigReal::_flt_max) > 0) {
    throwBigRealGetIntegralTypeOverflowException(method, x, toString(BigReal::_flt_max));
  }
  if(compareAbs(x,BigReal::_flt_min) < 0) {
    throwBigRealGetIntegralTypeUnderflowException(method, x, toString(BigReal::_flt_min));
  }
  return x.getFloatNoLimitCheck();
}
