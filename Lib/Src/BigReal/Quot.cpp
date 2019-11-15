#include "pch.h"

BigReal fmod(const BigReal &x, const BigReal &y, DigitPool *digitPool) {
  _SELECTDIGITPOOL(x);
  BigReal remainder(pool);
#ifdef IS32BIT
  quotRemainder(x, y, NULL, &remainder);
#else
  quotRemainder128(x, y, NULL, &remainder);
#endif
  return remainder;
}

BigReal operator%(const BigReal &x, const BigReal &y) {
  return fmod(x, y, x.getDigitPool());
}

BigReal &BigReal::operator%=(const BigReal &x) {
  return *this = *this % x;
}

BigReal quot(const BigReal &x, const BigReal &y, const BigReal &f, DigitPool *digitPool) {
  VALIDATETOLERANCE(f)
  _SELECTDIGITPOOL(x);
  BigReal result(pool);
  if(!BigReal::checkIsNormalQuotient(x, y, &result, NULL)) {
    return result;
  }

  if(BigReal::isPow10(y)) {
    result = x;
    if(y.isNegative()) {
      result.changeSign();
    }
    return result.multPow10(-BigReal::getExpo10(y));
  }

  if(BigReal::compareAbs(x,y) == 0) {
    return BigReal(sign(x) * sign(y), pool);
  }
#ifdef IS32BIT
  return BigReal::quotLinear64(x,y,f, pool);
#else
  return BigReal::quotLinear128(x,y,f, pool);
#endif

//  return BigReal::quotNewton(x,y,f, pool);
//  return BigReal::chooseQuotNewton(x,y,f,pool) ? BigReal::quotNewton(x,y,f,pool) : BigReal::quotLinear32(x,y,f,pool);
}


double BigReal::estimateQuotNewtonTime(const BigReal &x, const BigReal &y, const BigReal &f) { // static
  BRExpoType resultExpo   = getExpo10(x) - getExpo10(y); // Expected exponent of result
  BRExpoType errorExpo    = getExpo10(f);
  double     resultDigits = (double)(resultExpo - errorExpo);
  if(resultDigits <= 0) {
    return 1;
  }

  return ::pow(resultDigits,1.58496) / 1.18e7;  // 1.58496 = log2(3)
}

double BigReal::estimateQuotLinearTime(const BigReal &x, const BigReal &y, const BigReal &f) { // static
  BRExpoType resultExpo   = getExpo10(x) - getExpo10(y); // Expected exponent of result
  BRExpoType errorExpo    = getExpo10(f);
  intptr_t   resultDigits = resultExpo - errorExpo;
  int        yDigits      = getDecimalDigitCount(y.m_first->n);
  double estimate  = (1.51e-7*resultDigits - 6.38e-6 + ((-2.08e-7*yDigits + 3.47e-6)*yDigits));
  return max(0,estimate);
}

bool BigReal::chooseQuotNewton(const BigReal &x, const BigReal &y, const BigReal &f) { // static
  BRExpoType resultExpo   = getExpo10(x) - getExpo10(y); // Expected exponent of result
  BRExpoType errorExpo    = getExpo10(f);
  double     resultDigits = (double)(resultExpo - errorExpo);
  if(resultDigits <= 0) {
    return false;
  }
  if(y.getLength() > 1) {
    return true;
  }
  double newtonTime = ::pow(resultDigits,1.58496) / 1.18e7;
  double linearTime = resultDigits / 4.6e8;

  return newtonTime < linearTime;
}
