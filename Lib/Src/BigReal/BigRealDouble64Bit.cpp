#include "pch.h"
#include <ctype.h>

#define SIGNIFICANT_EXPO10 15

void BigReal::init(double x) {
  init();
  if(!isnormal(x)) {
    if(isnan(x)) {
      setToNan();
    } else if(isinf(x)) {
      setToInf();
      if(getSign(x) != 0) {
        changeSign();
      }
    }
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

bool isDouble(const BigReal &v) {
  if(!isnormal(v)) return true;
  if((compareAbs(v, ConstBigReal::_dbl_max) > 0) || (compareAbs(v, ConstBigReal::_dbl_min) < 0)) {
    return false;
  }
  const double d = v.getDoubleNoLimitCheck();
  return BigReal(d,v.getDigitPool()) == v;
}

double getDouble(const BigReal &x) {
  DEFINEMETHODNAME;
  if(!isnormal(x)) {
    if(x.isZero()) {
      return 0;
    } else if(isnan(x)) {
      return DBL_NAN;
    } else if(isPInfinity(x)) {
      return DBL_PINF;
    } else {
      return DBL_NINF;
    }
  }
  if(compareAbs(x,ConstBigReal::_dbl_max) > 0) {
    throwBigRealGetIntegralTypeOverflowException(method, x, toString(ConstBigReal::_dbl_max));
  }
  if(compareAbs(x,ConstBigReal::_dbl_min) < 0) {
    throwBigRealGetIntegralTypeUnderflowException(method, x, toString(ConstBigReal::_dbl_min));
  }
  return x.getDoubleNoLimitCheck();
}
