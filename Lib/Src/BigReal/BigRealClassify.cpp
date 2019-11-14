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
