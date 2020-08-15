#include "pch.h"

#define _0 pool->_0()
#define _1 pool->_1()

#if defined(_DEBUG)
#define APCAdjust(bias) adjustAPCResult(bias, __TFUNCTION__)
#define THROWINVALIDBIAS(bias) throwInvalidBias(function,bias)
static void throwInvalidBias(const TCHAR *function, char bias) {
  throwBigRealException(_T("%s:Invalid bias:'%c'. Must be <,> or #"), function, bias);
}
#else
#define APCAdjust(bias) adjustAPCResult(bias)
#define THROWINVALIDBIAS(bias) throwInvalidBias(bias)
static void throwInvalidBias(char bias) {
  throwBigRealException(_T("Invalid bias:'%c'. Must be <,> or #"), bias);
}
#endif

BigReal BigReal::apcSum(const char bias, const BigReal &x, const BigReal &y, DigitPool *digitPool) {
  _SELECTDIGITPOOL(x);
  if(!x._isfinite() || !y._isfinite()) return pool->nan();
  if(x.isZero()) {
    return y.isZero() ? _0 : BigReal(y, pool).rRound(APC_DIGITS+2).APCAdjust(bias);
  } else if(y.isZero()) { // x != 0
    return BigReal(x, pool).rRound(APC_DIGITS + 2).APCAdjust(bias);
  }

  const BRExpoType xe = getExpo10(x);
  const BRExpoType ye = getExpo10(y);
  return sum(x, y, e(_1, max(xe, ye) - APC_DIGITS - 2), pool).rRound(APC_DIGITS+2).APCAdjust(bias);
}

BigReal BigReal::apcProd(const char bias, const BigReal &x, const BigReal &y, DigitPool *digitPool) { // static
  _SELECTDIGITPOOL(x);
  BigReal result(pool);
  return result.checkIsNormalProduct(x, y)
       ? result.shortProductNoNormalCheck(x, y, x.m_expo + y.m_expo).rTrunc(APC_DIGITS + 2).APCAdjust(bias)
       : result;
}

#define MAXDIGITS_INT64     19
#define MAXDIGITS_DIVISOR64 ((MAXDIGITS_INT64+1)/2)

BigReal BigReal::apcQuot(const char bias, const BigReal &x, const BigReal &y, DigitPool *digitPool) {
  _SELECTDIGITPOOL(x);
  BigReal result(pool);
  if(!checkIsNormalQuotient(x, y, &result, NULL)) {
    return result;
  }

  BigReal z(pool);
  z.copyrTrunc(x, MAXDIGITS_INT64).clrFlags(BR_NEG);

  BRExpoType scale;
  const UINT64 yFirst = y.getFirst64(MAXDIGITS_DIVISOR64, &scale);

  BigReal tmp(pool), t(pool);
  for(int i = 0;; i++) {
    t.approxQuot64Abs(z, yFirst, scale).copySign(z); // sign(t) == sign(z)
    result += t;
    if(i == 1) break;
    z -= tmp.shortProductNoZeroCheck(t, y, 4).copySign(t); // ignore sign(y)
    if(z.isZero()) break;
  }
  return result.setSignByProductRule(x, y).rTrunc(APC_DIGITS+2).APCAdjust(bias);
}

BigReal BigReal::apcAbs(const char bias, const BigReal &x, DigitPool *digitPool) {
  _SELECTDIGITPOOL(x);
  if(!x._isnormal()) return BigReal(x, pool);
  return BigReal(pool).copyrTrunc(x, APC_DIGITS + 2).clrFlags(BR_NEG).APCAdjust(bias);
}

BigReal BigReal::apcPow(const char bias, const BigReal &x, const BigInt &y, DigitPool *digitPool) {
  _SELECTDIGITPOOL(x);
  BigReal result(pool);

  if(!result.checkIsNormalPow(x, y)) {
    return result;
  }

  result = _1;
  BigReal tmpX(x,pool), tmp(pool);

#define SHORTPROD(a,b) tmp.shortProductNoZeroCheck(a,b,4)

  const bool yNegative = y.isNegative();
  if(compareAbs(y, BigReal::_ui32_max) <= 0) {       // use unsigned long as exponent;
    for(ULONG tmpY = yNegative ? (ULONG)(-y) : (ULONG)y; tmpY;) {
      if(tmpY & 1) {
        result = SHORTPROD(result, tmpX); --tmpY;
      } else {
        tmpX   = SHORTPROD(tmpX  , tmpX); tmpY /= 2;
      }
    }
  } else if(compareAbs(y, BigReal::_ui64_max) <= 0) { // use unsigned __int64 as exponent
    for(UINT64 tmpY = yNegative ? (UINT64)(-y) : (UINT64)y; tmpY;) {
      if(tmpY & 1) {
        result = SHORTPROD(result, tmpX); --tmpY;
      } else {
        tmpX   = SHORTPROD(tmpX  , tmpX); tmpY /= 2;
      }
    }
  } else {                                                // use BigInt all the way down. guess this will almost never happen
    BigInt tmpY(y, digitPool);
    if(yNegative) tmpY.flipSign();
    while(!tmpY.isZero()) {
      if(isOdd(tmpY)) {
        result = SHORTPROD(result, tmpX); --tmpY;
      } else {
        tmpX   = SHORTPROD(tmpX, tmpX); tmpY.divide2();
      }
    }
  }
  if(yNegative) {
    return apcQuot(bias, _1, result);
  } else {
    return result.rTrunc(APC_DIGITS+2).APCAdjust(bias);
  }
}

#if defined(_DEBUG)
BigReal &BigReal::adjustAPCResult(const char bias, const TCHAR *function) {
#else
BigReal &BigReal::adjustAPCResult(const char bias) {
#endif
  if(bias == '#') {
    return *this;
  } else {
    DigitPool *pool = &m_digitPool;
    BigReal f(_1);
    f.multPow10(getExpo10(*this) - APC_DIGITS - 1, true);
    switch(bias) {
    case '<': *this -= f; break;
    case '>': *this += f; break;
    default : THROWINVALIDBIAS(bias);
    }
  }
  return *this;
}
