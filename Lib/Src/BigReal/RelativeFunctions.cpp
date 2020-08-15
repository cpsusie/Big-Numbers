#include "pch.h"
#include <limits.h>

// Calculates with relative precision ie. with the specified number of decimal digits

#define _0 pool->_0()
#define _1 pool->_1()

BigReal rSum(const BigReal &x, const BigReal &y, size_t digits, DigitPool *digitPool) {
  _SELECTDIGITPOOL(x);
  const BRExpoType xe = BigReal::getExpo10(x);
  const BRExpoType ye = BigReal::getExpo10(y);
  return sum(x,y,e(_1,max(xe,ye) - (BRExpoType)digits - 2,pool),pool);
}

BigReal rDif(const BigReal &x, const BigReal &y, size_t digits, DigitPool *digitPool) {
  _SELECTDIGITPOOL(x);
  const BRExpoType xe = BigReal::getExpo10(x);
  const BRExpoType ye = BigReal::getExpo10(y);
  return dif(x,y,e(_1,max(xe,ye) - (BRExpoType)digits - 2,pool),pool);
}

BigReal rProd(const BigReal &x, const BigReal &y, size_t digits, DigitPool *digitPool) {
  _SELECTDIGITPOOL(x);
  return prod(x,y,e(_1,BigReal::getExpo10(x) + BigReal::getExpo10(y) - (BRExpoType)digits - 2,pool),pool);
}

BigReal rQuot(const BigReal &x, const BigReal &y, size_t digits, DigitPool *digitPool) {
  _SELECTDIGITPOOL(x);
  return quot(x,y,e(_1,BigReal::getExpo10(x) - BigReal::getExpo10(y) - (BRExpoType)digits - 2,pool),pool);
}

BigReal rSqrt(const BigReal &x, size_t digits, DigitPool *digitPool) {
  _SELECTDIGITPOOL(x);
  return sqrt(x,e(_1, BigReal::getExpo10(x)/2 - (BRExpoType)digits,pool),pool);
}

class rExpConstants {
public:
  const ConstBigReal c1;
  const ConstBigReal c2;

  rExpConstants()
    :c1(_T("0.43429448190325183")) // 0.434294481903251827 = 1/ln(10) round up
    ,c2(_T("0.43429448190325182")) //                        1/ln(10) round down
  {
  }
};

static const rExpConstants REXPC;

#if defined(IS32BIT)
#define CD_MAX INT_MAX
#define CD_MIN INT_MIN
#else
#define CD_MAX LLONG_MAX
#define CD_MIN LLONG_MIN
#endif // IS32BIT

BigReal rExp(const BigReal &x, size_t digits, DigitPool *digitPool) {
  DEFINEMETHODNAME;
  _SELECTDIGITPOOL(x);
  const BigReal c = x.isNegative() ? -rProd(REXPC.c1,fabs(x,pool),20, pool) : rProd(REXPC.c2,x,20,pool);
  double cd;
  if(BigReal::compareAbs(x, pool->_05()) < 0) {
    cd = 0;
  } else {
    cd = (double)c;
    if(cd > CD_MAX) {
      throwBigRealInvalidArgumentException(method, _T("Argument too big"));
    } else if(cd < CD_MIN) {
      throwBigRealInvalidArgumentException(method, _T("Argument too small"));
    }
  }
  return exp(x, e(_1, (intptr_t)cd - digits - 1,pool),pool);
}

static BigReal getLogError(const BigReal &x, intptr_t digits, DigitPool *digitPool) {
  static const BigReal &c1 = BigReal::_2;
  static const BigReal &c2 = BigReal::_05;

  _SELECTDIGITPOOL(x);

  if(x > c1) {        // 2 < x
    return e(BigReal(BigReal::getExpo10(x)+1,pool),-digits,pool);
  } else if(x > _1) { // 1 < x <= 2
    return e(_1, BigReal::getExpo10(dif(x,_1,_0,pool)) - digits,pool);
  } else if(x > c2) { // 0.5 < x <= 1
    return e(_1, BigReal::getExpo10(dif(_1,x,_0,pool)) - digits,pool);
  } else {            // x <= 0.5
    return e(-BigReal::getExpo10N(x,pool), -digits,pool);
  }
}

BigReal rLn(const BigReal &x, size_t digits, DigitPool *digitPool) {
  _SELECTDIGITPOOL(x);
  return ln(x, getLogError(x, digits, pool),pool);
}

BigReal rLog(const BigReal &base, const BigReal &x, size_t digits, DigitPool *digitPool) { // log(x) base
  _SELECTDIGITPOOL(x);
  const BigReal lBase = rLn(base, digits+8,pool);
  const BigReal lX    = rLn(x   , digits+8,pool);
  return rQuot(lX,lBase,digits+8,pool);
}

BigReal rLog10(const BigReal &x, size_t digits, DigitPool *digitPool) { // log(x) base 10
  _SELECTDIGITPOOL(x);
  return log10(x, getLogError(x, digits,pool),pool);
}

BigReal rPow(const BigReal &x, const BigReal &y, size_t digits, DigitPool *digitPool) { // x^y
  _SELECTDIGITPOOL(x);
  if(y.isZero()) {
    return x.isZero() ? pool->nan() : _1;
  }
  if(y == _1) {
    return BigReal(x,pool);
  }
  if(x.isZero()) {
    return y.isNegative() ? pool->pinf() : _0;
  }
  if(x.isPositive()) {
    return rExp(rProd(y,rLn(x,digits+8,pool),digits+8,pool),digits+8,pool);
  }

  if(isEven(y)) {
    return rPow(-x,y,digits,pool);
  }
  if(isOdd(y)) {
    return -rPow(-x,y,digits,pool);
  }
  return pool->nan();
}

BigReal rRoot(const BigReal &x, const BigReal &y, size_t digits, DigitPool *digitPool) {
  _SELECTDIGITPOOL(x);
  if(!x.isNegative()) {
    return rPow(x,rQuot(_1,y,digits,pool),digits,pool);
  }
  if(isOdd(y)) {
    return -rPow(-x,rQuot(_1,y,digits,pool),digits,pool);
  }
  return pool->nan();
}

BigReal rSin(const BigReal &x, size_t digits, DigitPool *digitPool) {
  _SELECTDIGITPOOL(x);
  if(x.isZero()) {
    return _0;
  }

  const BRExpoType piee = min(0,BigReal::getExpo10(x)) - (x.getDecimalDigits() + digits); // Exponent of precision of pi-value
  const BigReal Pi = pi(e(_1,piee - 8,pool),pool);
  const BigReal xp = fmod(x,Pi, pool);

  return sin(x,e(_1,BigReal::getExpo10(xp) - digits,pool),pool);
}

BigReal rCos(const BigReal &x, size_t digits, DigitPool *digitPool) {
  _SELECTDIGITPOOL(x);
  static const BigReal &c1 = BigReal::_05;

  if(x.isZero()) {
    return _1;
  }

  const BRExpoType piee = -((intptr_t)x.getDecimalDigits() + (intptr_t)digits); // Exponent of precision of pi-value
  const BigReal Pih = pi(e(_1,piee - 8,pool),pool) * c1;
  const BigReal xp = fmod(x, Pih,pool);

  return cos(x,e(_1,BigReal::getExpo10(xp) - digits,pool),pool);
}

BigReal rTan(const BigReal &x, size_t digits, DigitPool *digitPool) {
  _SELECTDIGITPOOL(x);
  const BigReal sn = rSin(x,digits,pool);
  const BigReal cs = rCos(x,2*digits,pool);
  return rQuot(sn,cs,digits,pool);
}

BigReal rCot(const BigReal &x, size_t digits, DigitPool *digitPool) {
  _SELECTDIGITPOOL(x);
  const BigReal cs = rCos(x,digits,pool);
  const BigReal sn = rSin(x,2*digits,pool);
  return rQuot(cs,sn,digits,pool);
}

BigReal rAsin(const BigReal &x, size_t digits, DigitPool *digitPool) {
  _SELECTDIGITPOOL(x);
  if(x.isZero()) {
    return _0;
  }
  return asin(x,e(_1, min(0,BigReal::getExpo10(x)) - digits - 8,pool),pool);
}

BigReal rAcos(const BigReal &x, size_t digits, DigitPool *digitPool) {
  _SELECTDIGITPOOL(x);
  if(x == _1) {
    return _0;
  }
  return acos(x,e(_1, BigReal::getExpo10(_1 - x)/2 - digits - 8,pool),pool);
}

BigReal rAtan(const BigReal &x, size_t digits, DigitPool *digitPool) {
  _SELECTDIGITPOOL(x);
  if(x.isZero()) {
    return _0;
  }
  return atan(x,e(_1, min(0,BigReal::getExpo10(x)) - digits - 8,pool),pool);
}

BigReal rAcot(const BigReal &x, size_t digits, DigitPool *digitPool) {
  static const BigReal &c1 = BigReal::_05;
  _SELECTDIGITPOOL(x);
  if(x.isZero()) {
    return pi(e(_1,-(int)digits - 8), pool) * c1;
  }
  return acot(x,e(_1, min(0,-BigReal::getExpo10(x)) - digits - 8,pool),pool);
}

BigReal rPi(size_t digits, DigitPool *digitPool) {
  BigReal dummy;
  _SELECTDIGITPOOL(dummy);
  return pi(e(_1, -(intptr_t)digits-2, pool),pool);
}

BigReal rRound(const BigReal &x, size_t digits, DigitPool *digitPool) {
  if(x.isZero()) {
    return x;
  }
  return round(x, digits-BigReal::getExpo10(x)-1);
}
