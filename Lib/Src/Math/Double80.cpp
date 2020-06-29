#include "pch.h"
#include <Math/Double80.h>
#include <Math/FPU.h>

#pragma check_stack(off)
#pragma warning(disable : 4073)
#pragma init_seg(lib)

class InitDouble80 {
public:
  inline InitDouble80() {
    Double80::initClass();
  }
};

void Double80::initClass() {
  if(sizeof(Double80) != 10) {
    throwException(_T("%s:Size of Double80 must be 10. Size=%zu."), __TFUNCTION__, sizeof(Double80));
  }
  FPU::init();
  FPU::clearExceptions();
  FPU::setPrecisionMode(FPU_HIGH_PRECISION);
}

static InitDouble80 initDouble80;

const Double80  Double80::_0( 0  );
const Double80  Double80::_05(0.5);
const Double80  Double80::_1(1);
const Double80 &Double80::_DBL80_EPSILON  = *(Double80*)(void*)"\x00\x00\x00\x00\x00\x00\x00\x80\xc0\x3f"; // 1.08420217248550443e-019;
const Double80 &Double80::_DBL80_MIN      = *(Double80*)(void*)"\x00\x00\x00\x00\x00\x00\x00\x80\x01\x00"; // 3.36210314311209209e-4932;
const Double80 &Double80::_DBL80_MAX      = *(Double80*)(void*)"\xff\xff\xff\xff\xff\xff\xff\xff\xfe\x7f"; // 1.18973149535723227e+4932
const Double80 &Double80::_DBL80_QNAN     = *(Double80*)(void*)"\x00\x00\x00\x00\x00\x00\x00\xc0\xff\xff"; // quiet NaN
const Double80 &Double80::_DBL80_SNAN     = *(Double80*)(void*)"\x00\x08\x00\x00\x00\x00\x00\x80\xff\xff"; // signaling NaN
const Double80 &Double80::_DBL80_PINF     = *(Double80*)(void*)"\x00\x00\x00\x00\x00\x00\x00\x80\xff\x7f"; // +infinity;
const Double80 &Double80::_DBL80_TRUE_MIN = *(Double80*)(void*)"\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00"; // denormalized minimal positive value
const Double80             DBL80_PI(         (BYTE*)"\x35\xc2\x68\x21\xa2\xda\x0f\xc9\x00\x40"); // pi
const Double80             DBL80_PI_05(      (BYTE*)"\x35\xc2\x68\x21\xa2\xda\x0f\xc9\xff\x3f"); // pi/2

int _fpclass(const Double80 &x) {
  switch(getExponent(x)) {
  case 0     :
    { const UINT64 s = getSignificand(x);
      if(s == 0) {
        return x.isPositive() ? _FPCLASS_PZ : _FPCLASS_NZ;
      } else {
        return x.isPositive() ? _FPCLASS_PD : _FPCLASS_ND;
      }
    }
  case 0x7fff: // NaN -> INFINITY and INDEFINITE
    { const UINT64 s = getSignificand(x);
      if(s == (1ui64 << 63)) { // +/-INFINITY
        return x.isPositive() ? _FPCLASS_PINF: _FPCLASS_NINF;
      } // either snan or qnan. snan if signbit=1 and f0=1, f1=0 and at least one of {f2..f63} = 1
      return (((s>>62) == 2) && (s & 0x3fffffffffffffffui64) && getSign(x)) ? _FPCLASS_SNAN : _FPCLASS_QNAN;
    }
  }
  return x.isPositive() ? _FPCLASS_PN : _FPCLASS_NN;
}

int fpclassify(const Double80 &v) {
  switch(getExponent(v)) {
  case 0x7fff:
    return (getSignificand(v) == (1ui64<<63)) ? FP_INFINITE : FP_NAN;
  case 0:
    return (getSignificand(v) == 0) ? FP_ZERO : FP_SUBNORMAL;
  default:
    return FP_NORMAL;
  }
}

Double80::Double80(float x) {
  switch(_fpclass(x)) {
  case _FPCLASS_PZ:
  case _FPCLASS_NZ:
    *this = _0;
    return;
  case _FPCLASS_QNAN:
    *this = std::numeric_limits<Double80>::quiet_NaN();
    return;
  case _FPCLASS_SNAN:
    *this = std::numeric_limits<Double80>::signaling_NaN();
    return;
  default:
    _D80FromFlt(*this, x);
    return;
  }
}

Double80::Double80(double x) {
  switch(_fpclass(x)) {
  case _FPCLASS_PZ:
  case _FPCLASS_NZ:
    *this = _0;
    return;
  case _FPCLASS_QNAN:
    *this = std::numeric_limits<Double80>::quiet_NaN();
    return;
  case _FPCLASS_SNAN:
    *this = std::numeric_limits<Double80>::signaling_NaN();
    return;
  default:
    _D80FromDbl(*this, x);
    return;
  }
}

float  getFloat(const Double80 &x) {
  switch(_fpclass(x)) {
  case _FPCLASS_PZ  :
  case _FPCLASS_NZ  : return 0.0f;
  case _FPCLASS_QNAN: return std::numeric_limits<float>::quiet_NaN();
  case _FPCLASS_SNAN: return std::numeric_limits<float>::signaling_NaN();
  default           : return _D80ToFlt(x);
  }
}

double getDouble(const Double80 &x) {
  switch(_fpclass(x)) {
  case _FPCLASS_PZ  :
  case _FPCLASS_NZ  : return 0.0;
  case _FPCLASS_QNAN: return std::numeric_limits<double>::quiet_NaN();
  case _FPCLASS_SNAN: return std::numeric_limits<double>::signaling_NaN();
  default           : return _D80ToDbl(x);
  }
}

Double80 pow(const Double80 &x, const Double80 &y) {
  switch(sign(y)) {
  case  0: return Double80::_1;
  case -1: // y < 0
    if(x.isZero()) { // 0^negative = +inf
      return std::numeric_limits<Double80>::infinity();
    }
    // continue case
  default: // (y > 0) || (x != 0)
    Double80 tmp(x);
    switch(sign(x)) {
    case  0: return Double80::_0; // 0^positive = 0
    case -1:
      if(y == floor(y)) { // y is integer
        tmp = -tmp; // tmp > 0
        _D80pow(tmp, y); // tmp = |x|^y
        const INT64 d = getInt64(y);
        return isEven(d) ? tmp : -tmp; // sign of result depends on parity of y, neg for odd y, pos for even y
      }
      return std::numeric_limits<Double80>::quiet_NaN(); // negative^(non-integer) = nan
    case 1:
      _D80pow(tmp, y);
      return tmp;
    default: NODEFAULT;
    }
  }
  return std::numeric_limits<Double80>::quiet_NaN(); // should not come here
}

Double80 mypow(const Double80 &x, const Double80 &y) {
  switch(sign(y)) {
  case 0: // y == 0
    return x.isZero()
         ? std::numeric_limits<Double80>::quiet_NaN()
         : Double80::_1;
  case -1: // y < 0
    if(x.isZero()) { // 0^negative = +inf
      return std::numeric_limits<Double80>::infinity();
    }
    // continue case
  default: // (y > 0) || (x != 0)
    Double80 tmp(x);
    switch(sign(x)) {
    case  0:
      return Double80::_0; // 0^positive = 0
    case -1:
      if(y == floor(y)) { // y is integer
        tmp = -tmp; // tmp > 0
        _D80pow(tmp, y); // tmp = |x|^y
        const INT64 d = getInt64(y);
        return isEven(d) ? tmp : -tmp; // sign of result depends on parity of y, neg for odd y, pos for even y
      }
      return std::numeric_limits<Double80>::quiet_NaN(); // negative^(non-integer) = nan
    case 1:
      _D80pow(tmp, y);
      return tmp;
    default: NODEFAULT;
    }
  }
  return std::numeric_limits<Double80>::quiet_NaN(); // should not come here
}

Double80 root(const Double80 &x, const Double80 &y) {
  if(x.isNegative()) {
    if(y == floor(y)) {
      const INT64 d = getInt64(y);
      if(isOdd(d)) {
        return -pow(-x, Double80::_1/y);
      }
    }
  }
  return pow(x, Double80::_1/y);
}

Double80 asin(const Double80 &x) {
  if(x == 1) {
    return DBL80_PI_05;
  } else if(x == -1) {
    return -DBL80_PI_05;
  } else {
    return atan2(x,sqrt(1.0-x*x));
  }
}

Double80 fraction(const Double80 &x) {
  if(x.isZero()) {
    return x;
  } else if(x.isNegative()) {
    return -fraction(-x);
  } else {
    return x - floor(x);
  }
}

Double80 round(const Double80 &x, int dec) { // 5-rounding
  const int sx = sign(x);
  switch(sx) {
  case 0:
    return Double80::_0;
  case  1:
  case -1:
    switch(sign(dec)) {
    case 0:
      return (sx == 1) ? floor(0.5+x) : -floor(0.5-x);
    case 1 :
      { Double80 p = Double80::pow10(dec);
        const FPUControlWord cwSave = FPU::setRoundMode(FPU_ROUNDCONTROL_ROUND);
        Double80 result = (sx == 1) ? floor(0.5+x*p) : -floor(0.5-x*p);
        result /= p;
        FPU::restoreControlWord(cwSave);
        return result;
      }
    case -1:
      { Double80 p = Double80::pow10(-dec);
        const FPUControlWord cwSave = FPU::setRoundMode(FPU_ROUNDCONTROL_ROUND);
        Double80 result = (sx == 1) ? floor(0.5+x/p) : -floor(0.5-x/p);
        result *= p;
        FPU::restoreControlWord(cwSave);
        return result;
      }
    }
  }
  throwException(_T("round dropped to the end. x=%s. dec=%d"),toString(x).cstr(),dec);
  return x; // Should never come here
}

ULONG Double80::hashCode() const {
  return *(ULONG*)m_value
       ^ *(ULONG*)(m_value+4)
       ^ *(USHORT*)(m_value+8);
}
