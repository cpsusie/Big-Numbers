#include "pch.h"
#include <ctype.h>

void BigReal::init(float x) {
  init();

  if(x != 0) {
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
