#include "pch.h"

BigReal operator%(const BigReal &x, const BigReal &y) {
  DigitPool *pool = x.getDigitPool();
  BigReal remainder(pool);
  quotRemainder(x,y,NULL, &remainder);
  return remainder;
}

BigReal &BigReal::operator%=(const BigReal &x) {
  return *this = *this % x;
}

BigReal quot(const BigReal &x, const BigReal &y, const BigReal &f, DigitPool *digitPool) {
  DEFINEMETHODNAME(quot);
  if(y.isZero()) {
    throwBigRealInvalidArgumentException(method, _T("Division by zero"));
  }
  if(!f.isPositive()) {
    throwInvalidToleranceException(method);
  }
  DigitPool *pool = digitPool ? digitPool : x.getDigitPool();

  if(x.isZero()) {
    return pool->get0();
  }

  if(BigReal::isPow10(y)) {
    BigReal result(x, pool);
    if(y.isNegative()) {
      result.changeSign();
    }
    return result.multPow10(-BigReal::getExpo10(y));
  }

  if(compareAbs(x,y) == 0) {
    return BigReal(sign(x) * sign(y), pool);
  }

  return BigReal::quotLinear64(x,y,f, pool);
//  return BigReal::quotNewton(x,y,f, pool);
//  return BigReal::chooseQuotNewton(x,y,f,pool) ? BigReal::quotNewton(x,y,f,pool) : BigReal::quotLinear32(x,y,f,pool);
}


double BigReal::estimateQuotNewtonTime(const BigReal &x, const BigReal &y, const BigReal &f) { // static
  int resultExpo = getExpo10(x) - getExpo10(y); // Expected exponent of result
  int errorExpo  = getExpo10(f);
  double resultDigits = resultExpo - errorExpo;
  if(resultDigits <= 0) {
    return 1;
  }

  return ::pow(resultDigits,1.58496) / 1.18e7;  // 1.58496 = log2(3)
}

double BigReal::estimateQuotLinearTime(const BigReal &x, const BigReal &y, const BigReal &f) { // static
  int resultExpo   = getExpo10(x) - getExpo10(y); // Expected exponent of result
  int errorExpo    = getExpo10(f);
  int resultDigits = resultExpo - errorExpo;
  int yDigits      = getDecimalDigitCount(y.m_first->n);
  double estimate  = (1.51e-7*resultDigits - 6.38e-6 + ((-2.08e-7*yDigits + 3.47e-6)*yDigits));
  return max(0,estimate);
}

bool BigReal::chooseQuotNewton(const BigReal &x, const BigReal &y, const BigReal &f) { // static
  int resultExpo = getExpo10(x) - getExpo10(y); // Expected exponent of result
  int errorExpo  = getExpo10(f);
  double resultDigits = resultExpo - errorExpo;
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

