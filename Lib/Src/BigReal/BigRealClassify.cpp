#include "pch.h"

// returns one of
// _FPCLASS_QNAN  0x0002   quiet NaN
// _FPCLASS_NINF  0x0004   negative infinity
// _FPCLASS_NN    0x0008   negative normal
// _FPCLASS_PZ    0x0040   +0
// _FPCLASS_PN    0x0100   positive normal
// _FPCLASS_PINF  0x0200   positive infinity
int _fpclass(const BigReal &x) {
  if (x.m_expo == BIGREAL_NONNORMAL) {
    switch (x.m_low) {
    case BIGREAL_ZEROLOW: return _FPCLASS_PZ;
    case BIGREAL_INFLOW: return x.isNegative() ? _FPCLASS_NINF : _FPCLASS_PINF;
    case BIGREAL_QNANLOW: return _FPCLASS_QNAN;
    default: NODEFAULT;
    }
  }
  else {
    return x.isNegative() ? _FPCLASS_NN : _FPCLASS_PN;
  }
}

BigReal &BigReal::setToNonNormalFpClass(int fpclass) {
  switch(fpclass) {
  case _FPCLASS_NZ  :
  case _FPCLASS_PZ  : return setToZero();
  case _FPCLASS_SNAN:
  case _FPCLASS_QNAN: return setToNan();
  case _FPCLASS_PINF: return setToPInf();
  case _FPCLASS_NINF: return setToNInf();
  default           : throwInvalidArgumentException(__TFUNCTION__, _T("fpclass=%04x"), fpclass);
  }
  return *this;
}

// Return _FP_ZERO, _FPCLASS_QNAN
// Assume !x._isnormal() || !y._isnormal() which will result in non-normal product (0 or nan)
int BigReal::getNonNormalProductFpClass(const BigReal &x, const BigReal &y) { // static
  assert(!x._isnormal() || !y._isnormal());
  if(!x._isfinite() || !y._isfinite()) {
    return _FPCLASS_QNAN;
  }
  return _FPCLASS_PZ;
}

// Assume !x._isnormal() || !y._isnormal() return _FPCLASS_PZ, _FPCLASS_PINF, _FPCLASS_NINF, _FPCLASS_QNAN
int BigReal::getNonNormalQuotientFpClass(const BigReal &x, const BigReal &y) { // static
  assert(!x._isnormal() || !y._isnormal());
  if (!x._isfinite() || !y._isfinite()) {
    return _FPCLASS_QNAN;
  }
  if (y.isZero()) {
    switch (_fpclass(x)) {
    case _FPCLASS_PN: return _FPCLASS_PINF;
    case _FPCLASS_NN: return _FPCLASS_NINF;
    default: return _FPCLASS_QNAN;
    }
  }
  if (x.isZero()) return _FPCLASS_PZ;
  // should never come here
  throwException(_T("%s:_fpclass(x):%d, _fpclass(y):%d"), __TFUNCTION__, _fpclass(x), _fpclass(y));
  return _FPCLASS_QNAN;
}

void BigReal::validateQuotRemainderArguments(const TCHAR *method, const BigReal &x, const BigReal &y, const BigReal *quotient, const BigReal *remainder) { // static
  if(quotient == remainder) { // also takes care of the stupid situation where both are NULL
    throwBigRealInvalidArgumentException(method, _T("Quotient is the same variable as remainder"));
  }
  if((quotient == &x) || (quotient == &y)) {
    throwBigRealInvalidArgumentException(method, _T("Quotient cannot be the same variable as x or y"));
  }
  if((remainder == &x) || (remainder == &y)) {
    throwBigRealInvalidArgumentException(method, _T("Remainder cannot be the same variable as x or y"));
  }
}

// _FPCLASS_PN, _FPCLASS_NN, _FPCLASS_PZ, _FPCLASS_PINF, _FPCLASS_QNAN
int BigReal::getPowFpClass(const BigReal &x, const BigReal &y) {
  if(!x._isfinite() || !y._isfinite()) {
    return _FPCLASS_QNAN;
  }
  if(x.isZero()) {
    return y.isZero() ? _FPCLASS_QNAN : y.isNegative() ? _FPCLASS_PINF : _FPCLASS_PZ;
  }
  // x is finite and x != 0
  if(x.isNegative()) {
    if(!y._isinteger()) return _FPCLASS_QNAN;
    return isEven(y) ? _FPCLASS_PN : _FPCLASS_NN;
  }
  return _FPCLASS_PN;
}

bool BigReal::checkIsNormalPow(const BigReal &x, const BigReal &y) {
  int cl;
  switch(cl = getPowFpClass(x,y)) {
  case _FPCLASS_PN:
    if(y.isZero()) {
      *this = m_digitPool._1();
      return false;
    }
    // continue case
  case _FPCLASS_NN:
    return true;
  default:
    setToNonNormalFpClass(cl);
    return false;
  }
}
