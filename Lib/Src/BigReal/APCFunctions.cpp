#include "pch.h"

BigReal BigReal::apcSum(const char bias, const BigReal &x, const BigReal &y, DigitPool *digitPool) {
  DEFINEMETHODNAME;
  if(digitPool == NULL) digitPool = x.getDigitPool();

  HANDLE_INFBINARYOP(x,y,digitPool);

  if(x.isZero()) {
    return BigReal(y, digitPool);
  } else if(y.isZero()) {
    return BigReal(x, digitPool);
  }

  const BRExpoType xe = getExpo10(x);
  const BRExpoType ye = getExpo10(y);
  return sum(x, y, e(digitPool->_1(), max(xe, ye) - APC_DIGITS - 2), digitPool).rRound(APC_DIGITS+2).adjustAPCResult(bias, method);
}

BigReal BigReal::apcProd(const char bias, const BigReal &x, const BigReal &y, DigitPool *digitPool) { // static
  if(digitPool == NULL) digitPool = x.getDigitPool();

  BigReal result(digitPool);
  result.shortProduct(x, y, x.m_expo + y.m_expo);
  if(result._isnormal()) {
    result.rTrunc(APC_DIGITS + 2).adjustAPCResult(bias, __TFUNCTION__);
  }
  return result;
}

#define MAXDIGITS_INT64     19
#define MAXDIGITS_DIVISOR64 ((MAXDIGITS_INT64+1)/2)

BigReal BigReal::apcQuot(const char bias, const BigReal &x, const BigReal &y, DigitPool *digitPool) {
  DEFINEMETHODNAME;
  if(digitPool == NULL) digitPool = x.getDigitPool();

  BigReal result(digitPool);
  if(!checkIsNormalQuotient(x, y, &result, NULL)) {
    return result;
  }
  const bool yNegative = y.isNegative();
  ((BigReal&)y).setPositive(); // cheating. We set it back agin

  BigReal z(digitPool);
  z.copyrTrunc(x, MAXDIGITS_INT64).setPositive();

  BRExpoType scale;
  const UINT64 yFirst = y.getFirst64(MAXDIGITS_DIVISOR64, &scale);

  BigReal tmp(digitPool), t(digitPool);
  for(int i = 0;; i++) {
    t.approxQuot64Abs(z, yFirst, scale).m_negative = z.m_negative;
    result += t;
    if(i == 1) break;
    z -= tmp.shortProductNoZeroCheck(t, y, 4);
    if(z.isZero()) break;
  }

  ((BigReal&)y).m_negative = yNegative;
  return result.rTrunc(APC_DIGITS+2).setSignByProductRule(x,y).adjustAPCResult(bias, method);
}

BigReal BigReal::apcPow(const char bias, const BigReal &x, const BigInt &y, DigitPool *digitPool) {
  DEFINEMETHODNAME;
  if(digitPool == NULL) digitPool = x.getDigitPool();

  HANDLE_INFBINARYOP(x,y,digitPool);

  static const BigReal &c1 = BigReal::_05;

  if(y.isZero()) {
    return digitPool->_1();
  } else if(x.isZero()) {
    if(y.isNegative()) {
      throwBigRealInvalidArgumentException(method, _T("x = 0 and y < 0"));
    }
    return digitPool->_0();
  } else {
    BigReal result = digitPool->_1();
    BigReal tmpX(x,digitPool), tmp(digitPool);

#define SHORTPROD(a,b) tmp.shortProductNoZeroCheck(a,b,4)

    const bool yNegative = y.isNegative();
    if(compareAbs(y, BigReal::_ui32_max) <= 0) {       // use unsigned long as exponent;
      for(unsigned long tmpY = yNegative ? getUlong(-y) : getUlong(y); tmpY;) {
        if(tmpY & 1) {
          result = SHORTPROD(result, tmpX); --tmpY;
        } else {
          tmpX   = SHORTPROD(tmpX  , tmpX); tmpY /= 2;
        }
      }
    } else if(compareAbs(y, BigReal::_ui64_max) <= 0) { // use unsigned __int64 as exponent
      for(unsigned __int64 tmpY = yNegative ? getUint64(-y) : getUint64(y); tmpY;) {
        if(tmpY & 1) {
          result = SHORTPROD(result, tmpX); --tmpY;
        } else {
          tmpX   = SHORTPROD(tmpX  , tmpX); tmpY /= 2;
        }
      }
    } else {                                                // use BigInt all the way down. guess this will almost never happen
      BigReal tmpY(y, digitPool);
      if(yNegative) tmpY.changeSign();
      while(!tmpY.isZero()) {
        if(odd(tmpY)) {
          result = SHORTPROD(result, tmpX); --tmpY;
        } else {
          tmpX   = SHORTPROD(tmpX  , tmpX); tmpY *= c1;
        }
      }
    }
    if(yNegative) {
      return apcQuot(bias, digitPool->_1(), result);
    } else {
      return result.rTrunc(APC_DIGITS+2).adjustAPCResult(bias, method);
    }
  }
}

static void throwInvalidBias(const TCHAR *function, char bias) {
  throwBigRealException(_T("%s:Invalid bias:'%c'. Must be <,> or #"), function, bias);
}

BigReal &BigReal::adjustAPCResult(const char bias, const TCHAR *function) {
  if(bias == '#') {
    return *this;
  } else {
    BigReal f(m_digitPool._1());
    f.multPow10(getExpo10(*this) - APC_DIGITS - 1);
    switch(bias) {
    case '<': *this -= f; break;
    case '>': *this += f; break;
    default : throwInvalidBias(function, bias);
    }
  }
  return *this;
}
