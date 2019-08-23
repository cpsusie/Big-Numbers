#include "StdAfx.h"
#include "FloatFields.h"
#include <Random.h>

void FloatFields::checkType(const TCHAR *method, FloatType expected) const {
  if(getType() != expected) {
    throwException(_T("%s:Wrong floatType:%s, expected:%s"), method, toString(getType()).cstr(), toString(expected).cstr());
  }
}

#define DEFAULT_WRONGTYPE(type)  default: throwInvalidArgumentException(__TFUNCTION__, _T("type=%d"), type)

float FloatFields::getFloat() const {
  checkType(__TFUNCTION__, FT_FLOAT);
  union {
    BYTE  v[4];
    UINT  i32;
    float f;
  };

  i32 = 0;
  i32 = (UINT)(m_sig & getSigMask());
  i32 |= (UINT)(m_expoField & getExpoMask()) << getExpoShift();
  if(m_sign) LASTVALUE(v) |= 0x80;
  return f;
}

double FloatFields::getDouble() const {
  checkType(__TFUNCTION__, FT_DOUBLE);
  union {
    BYTE   v[8];
    UINT64 i64;
    double d;
  };
  i64 = 0;
  i64 = m_sig & getSigMask();
  i64 |= (UINT64)(m_expoField & getExpoMask()) << getExpoShift();
  if(m_sign) LASTVALUE(v) |= 0x80;
  return d;
}

Double80 FloatFields::getDouble80() const {
  checkType(__TFUNCTION__, FT_DOUBLE80);
  BYTE v[10];
  memset(v, 0, sizeof(v));
  *((UINT64*)v) = m_sig;
  *((USHORT*)(v + 8)) |= m_expoField & getExpoMask();
  if(m_sign) LASTVALUE(v) |= 0x80;
  return Double80(v);
}

FloatFields &FloatFields::setType(FloatType type) {
  const FloatType oldType = getType();
  if(type != oldType) {
    switch(type) {
    case FT_FLOAT  :
      switch(oldType) {
      case FT_DOUBLE  : *this = ::getFloat(getDouble());    break;
      case FT_DOUBLE80: *this = ::getFloat(getDouble80());  break;
      }
      break;
    case FT_DOUBLE  :
      switch(oldType) {
      case FT_FLOAT   : *this = ::getDouble(getFloat());    break;
      case FT_DOUBLE80: *this = ::getDouble(getDouble80()); break;
      }
      break;
    case FT_DOUBLE80:
      switch(oldType) {
      case FT_FLOAT   : *this =   Double80(getFloat());     break;
      case FT_DOUBLE  : *this =   Double80(getDouble());    break;
      }
      break;
    }
  }
  return *this;
}

FloatFields &FloatFields::setZero() {
  switch(getType()) {
  case FT_FLOAT   : *this = 0.0f;         break;
  case FT_DOUBLE  : *this = 0.0;          break;
  case FT_DOUBLE80: *this = Double80::_0; break;
  DEFAULT_WRONGTYPE(getType());
  }
  return *this;
}

FloatFields &FloatFields::setTrueMin() {
  switch(getType()) {
  case FT_FLOAT   : *this = std::numeric_limits<float   >::denorm_min();  break;
  case FT_DOUBLE  : *this = std::numeric_limits<double  >::denorm_min();  break;
  case FT_DOUBLE80: *this = std::numeric_limits<Double80>::denorm_min();  break;
  DEFAULT_WRONGTYPE(getType());
  }
  return *this;
}

#undef min
FloatFields &FloatFields::setMin() {
  switch(getType()) {
  case FT_FLOAT   : *this = std::numeric_limits<float   >::min();      break;
  case FT_DOUBLE  : *this = std::numeric_limits<double  >::min();      break;
  case FT_DOUBLE80: *this = std::numeric_limits<Double80>::min();      break;
  DEFAULT_WRONGTYPE(getType());
  }
  return *this;
}

FloatFields &FloatFields::setEps() {
  switch(getType()) {
  case FT_FLOAT   : *this = std::numeric_limits<float   >::epsilon();  break;
  case FT_DOUBLE  : *this = std::numeric_limits<double  >::epsilon();  break;
  case FT_DOUBLE80: *this = std::numeric_limits<Double80>::epsilon();  break;
  DEFAULT_WRONGTYPE(getType());
  }
  return *this;
}

FloatFields &FloatFields::setRnd() {
  switch(getType()) {
  case FT_FLOAT   : *this = randFloat();    break;
  case FT_DOUBLE  : *this = randDouble();   break;
  case FT_DOUBLE80: *this = randDouble80(); break;
  DEFAULT_WRONGTYPE(getType());
  }
  return *this;
}

FloatFields &FloatFields::setOne() {
  switch(getType()) {
  case FT_FLOAT   : *this = 1.0f;         break;
  case FT_DOUBLE  : *this = 1.0;          break;
  case FT_DOUBLE80: *this = Double80::_1; break;
  DEFAULT_WRONGTYPE(getType());
  }
  return *this;
}

#undef max
FloatFields &FloatFields::setMax() {
  switch(getType()) {
  case FT_FLOAT   : *this = std::numeric_limits<float   >::max();      break;
  case FT_DOUBLE  : *this = std::numeric_limits<double  >::max();      break;
  case FT_DOUBLE80: *this = std::numeric_limits<Double80>::max();      break;
  DEFAULT_WRONGTYPE(getType());
  }
  return *this;
}

FloatFields &FloatFields::setPInf() {
  switch(getType()) {
  case FT_FLOAT   : *this = std::numeric_limits<float   >::infinity();       break;
  case FT_DOUBLE  : *this = std::numeric_limits<double  >::infinity();       break;
  case FT_DOUBLE80: *this = std::numeric_limits<Double80>::infinity();       break;
  DEFAULT_WRONGTYPE(getType());
  }
  return *this;
}

FloatFields &FloatFields::setQNaN() {
  switch(getType()) {
  case FT_FLOAT   : *this = std::numeric_limits<float   >::quiet_NaN();     break;
  case FT_DOUBLE  : *this = std::numeric_limits<double  >::quiet_NaN();     break;
  case FT_DOUBLE80: *this = std::numeric_limits<Double80>::quiet_NaN();     break;
  DEFAULT_WRONGTYPE(getType());
  }
  return *this;
}

FloatFields &FloatFields::setSNaN() {
  switch(getType()) {
  case FT_FLOAT   : *this = std::numeric_limits<float   >::signaling_NaN(); break;
  case FT_DOUBLE  : *this = std::numeric_limits<double  >::signaling_NaN(); break;
  case FT_DOUBLE80: *this = std::numeric_limits<Double80>::signaling_NaN(); break;
  DEFAULT_WRONGTYPE(getType());
  }
  return *this;
}

FloatFields &FloatFields::operator+=(const FloatFields &rhs) {
  checkType(__TFUNCTION__, rhs.getType());
  switch(getType()) {
  case FT_FLOAT   : *this = (float )(getFloat()    + rhs.getFloat()  ); break;
  case FT_DOUBLE  : *this = (double)(getDouble()   + rhs.getDouble() ); break;
  case FT_DOUBLE80: *this =          getDouble80() + rhs.getDouble80(); break;
  DEFAULT_WRONGTYPE(getType());
  }
  return *this;
}
FloatFields &FloatFields::operator-=(const FloatFields &rhs) {
  checkType(__TFUNCTION__, rhs.getType());
  switch(getType()) {
  case FT_FLOAT   : *this = (float )(getFloat()    - rhs.getFloat()  ); break;
  case FT_DOUBLE  : *this = (double)(getDouble()   - rhs.getDouble() ); break;
  case FT_DOUBLE80: *this =          getDouble80() - rhs.getDouble80(); break;
  DEFAULT_WRONGTYPE(getType());
  }
  return *this;
}
FloatFields &FloatFields::operator*=(const FloatFields &rhs) {
  checkType(__TFUNCTION__, rhs.getType());
  switch(getType()) {
  case FT_FLOAT   : *this = (float )(getFloat()    * rhs.getFloat()  ); break;
  case FT_DOUBLE  : *this = (double)(getDouble()   * rhs.getDouble() ); break;
  case FT_DOUBLE80: *this =          getDouble80() * rhs.getDouble80(); break;
  DEFAULT_WRONGTYPE(getType());
  }
  return *this;
}
FloatFields &FloatFields::operator/=(const FloatFields &rhs) {
  checkType(__TFUNCTION__, rhs.getType());
  switch(getType()) {
  case FT_FLOAT   : *this = (float )(getFloat()    / rhs.getFloat()  ); break;
  case FT_DOUBLE  : *this = (double)(getDouble()   / rhs.getDouble() ); break;
  case FT_DOUBLE80: *this =          getDouble80() / rhs.getDouble80(); break;
  DEFAULT_WRONGTYPE(getType());
  }
  return *this;
}

FloatFields &FloatFields::operator%=(const FloatFields &rhs) {
  checkType(__TFUNCTION__, rhs.getType());
  switch(getType()) {
  case FT_FLOAT   : *this = (float )(fmod(getFloat()    , rhs.getFloat())  ); break;
  case FT_DOUBLE  : *this = (double)(fmod(getDouble()   , rhs.getDouble()) ); break;
  case FT_DOUBLE80: *this =          fmod(getDouble80() , rhs.getDouble80()); break;
  DEFAULT_WRONGTYPE(getType());
  }
  return *this;
}

UINT FloatFields::getBitCount(FloatType type) { // static
  switch(type) {
  case FT_FLOAT   : return 32;
  case FT_DOUBLE  : return 64;
  case FT_DOUBLE80: return 80;
  DEFAULT_WRONGTYPE(type);
  }
  return 0;
}

UINT FloatFields::getExpoBitCount(FloatType type) { // static
  switch(type) {
  case FT_FLOAT   : return  8;
  case FT_DOUBLE  : return 11;
  case FT_DOUBLE80: return 15;
  DEFAULT_WRONGTYPE(type);
  }
  return 0;
}

int FloatFields::getMaxDigits10(FloatType type) {
  switch(type) {
  case FT_FLOAT   : return std::numeric_limits<float>::max_digits10;
  case FT_DOUBLE  : return std::numeric_limits<double>::max_digits10;
  case FT_DOUBLE80: return std::numeric_limits<Double80>::max_digits10;
  DEFAULT_WRONGTYPE(type);
  }
  return 0;
}

static String sprintbin(UINT64 n, UINT count) {
  String result;
  for(int i = count; i--;) {
    if((i % 4 == 3) && !result.isEmpty()) {
      result += ' ';
    }
    result += ((n >> i) & 1) ? '1' : '0';
  }
  return result;
}

static String dataToHexString(const void *p, size_t n) {
  String result;
  BYTE *pp = ((BYTE*)p) + n - 1;
  for(size_t i = 0; i < n; i++) {
    result += format(_T("%02X"), *(pp--));
  }
  for(int dotPos = (int)result.length() - 4; dotPos > 0; dotPos -= 4) {
    result.insert(dotPos, _T('.'));
  }
  return result;
}

String FloatFields::toBinString() const {
  return format(_T("Sign:%c   Expo:%-19s   Sig:%s")
               ,getSignBit() ? _T('1') : '0'
               ,sprintbin(getExpoField(), getExpoBitCount()).cstr()
               ,sprintbin(getSig(), getSigBitCount()).cstr());
}

String FloatFields::toHexString() const {
  const int         prec     = getMaxDigits10();
  switch(getType()) {
  case FT_FLOAT   :
    { const float    v = getFloat();
      return format(_T("Hex:%-24s  Value:%s"), dataToHexString(&v,sizeof(v)).cstr(), toString(v, prec).cstr());
    }
  case FT_DOUBLE  :
    { const double   v = getDouble();
      return format(_T("Hex:%-24s  Value:%s"), dataToHexString(&v, sizeof(v)).cstr(), toString(v, prec).cstr());
    }
  case FT_DOUBLE80:
    { const Double80 v = getDouble80();
      return format(_T("Hex:%-24s  Value:%s"), dataToHexString(&v, sizeof(v)).cstr(), toString(v, prec).cstr());
    }
  }
  return format(_T("Unknonwn floatType:%d"), getType());
}

int FloatFields::getFpClass() const {
  switch(getType()) {
  case FT_FLOAT   : return _fpclass((double)getFloat());
  case FT_DOUBLE  : return _fpclass(getDouble());
  case FT_DOUBLE80: return _fpclass(getDouble80());
  DEFAULT_WRONGTYPE(getType());
  }
  return 0;
}

String fpclassToString(int fpClass) {
  switch(fpClass) {
#define caseStr(v) case v: return _T(#v)
  caseStr(_FPCLASS_SNAN);
  caseStr(_FPCLASS_QNAN);
  caseStr(_FPCLASS_NINF);
  caseStr(_FPCLASS_NN  );
  caseStr(_FPCLASS_ND  );
  caseStr(_FPCLASS_NZ  );
  caseStr(_FPCLASS_PZ  );
  caseStr(_FPCLASS_PD  );
  caseStr(_FPCLASS_PN  );
  caseStr(_FPCLASS_PINF);
  default:
    return format(_T("Unknown floating point class:%#X"), fpClass);
  }
}

String toString(FloatType type) {
  switch(type) {
  caseStr(FT_FLOAT   );
  caseStr(FT_DOUBLE  );
  caseStr(FT_DOUBLE80);
  default:
    return format(_T("Unknown floating point type:%#X"), type);
  }
}

FloatFields sqr(const FloatFields &ff) {
  return ff * ff;
}

FloatFields reciproc(const FloatFields &ff) {
  FloatFields tmp(ff);
  tmp.setOne();
  return tmp / ff;
}

FloatFields sqrt(const FloatFields &ff) {
  switch(ff.getType()) {
  case FT_FLOAT:
    { const float f = ff.getFloat();
      return FloatFields((float)sqrtf(f));
    }
  case FT_DOUBLE  :
    { const double d = ff.getDouble();
      return FloatFields(sqrt(d));
    }
  case FT_DOUBLE80:
    { const Double80 d80 = ff.getDouble80();
      return FloatFields(sqrt(d80));
    }
  DEFAULT_WRONGTYPE(ff.getType());
  }
  return ff;
}
