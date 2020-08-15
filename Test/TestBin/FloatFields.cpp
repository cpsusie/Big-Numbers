#include "StdAfx.h"
#include "FloatFields.h"
#include <Random.h>

using namespace std;

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
      case FT_DOUBLE  : *this = (float)getDouble();    break;
      case FT_DOUBLE80: *this = (float)getDouble80();  break;
      }
      break;
    case FT_DOUBLE  :
      switch(oldType) {
      case FT_FLOAT   : *this = (double)getFloat();    break;
      case FT_DOUBLE80: *this = (double)getDouble80(); break;
      }
      break;
    case FT_DOUBLE80:
      switch(oldType) {
      case FT_FLOAT   : *this = Double80(getFloat());     break;
      case FT_DOUBLE  : *this = Double80(getDouble());    break;
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

FloatFields &FloatFields::setTrueMin() {
  switch(getType()) {
  case FT_FLOAT   : *this = numeric_limits<float   >::denorm_min();    break;
  case FT_DOUBLE  : *this = numeric_limits<double  >::denorm_min();    break;
  case FT_DOUBLE80: *this = numeric_limits<Double80>::denorm_min();    break;
  DEFAULT_WRONGTYPE(getType());
  }
  return *this;
}

FloatFields &FloatFields::setEps() {
  switch(getType()) {
  case FT_FLOAT   : *this = numeric_limits<float   >::epsilon();       break;
  case FT_DOUBLE  : *this = numeric_limits<double  >::epsilon();       break;
  case FT_DOUBLE80: *this = numeric_limits<Double80>::epsilon();       break;
  DEFAULT_WRONGTYPE(getType());
  }
  return *this;
}
#undef min
FloatFields &FloatFields::setMin() {
  switch(getType()) {
  case FT_FLOAT   : *this = numeric_limits<float   >::min();           break;
  case FT_DOUBLE  : *this = numeric_limits<double  >::min();           break;
  case FT_DOUBLE80: *this = numeric_limits<Double80>::min();           break;
  DEFAULT_WRONGTYPE(getType());
  }
  return *this;
}


#undef max
FloatFields &FloatFields::setMax() {
  switch(getType()) {
  case FT_FLOAT   : *this = numeric_limits<float   >::max();           break;
  case FT_DOUBLE  : *this = numeric_limits<double  >::max();           break;
  case FT_DOUBLE80: *this = numeric_limits<Double80>::max();           break;
  DEFAULT_WRONGTYPE(getType());
  }
  return *this;
}

FloatFields &FloatFields::setPInf() {
  switch(getType()) {
  case FT_FLOAT   : *this = numeric_limits<float   >::infinity();      break;
  case FT_DOUBLE  : *this = numeric_limits<double  >::infinity();      break;
  case FT_DOUBLE80: *this = numeric_limits<Double80>::infinity();      break;
  DEFAULT_WRONGTYPE(getType());
  }
  return *this;
}

FloatFields &FloatFields::setQNaN() {
  switch(getType()) {
  case FT_FLOAT   : *this = numeric_limits<float   >::quiet_NaN();     break;
  case FT_DOUBLE  : *this = numeric_limits<double  >::quiet_NaN();     break;
  case FT_DOUBLE80: *this = numeric_limits<Double80>::quiet_NaN();     break;
  DEFAULT_WRONGTYPE(getType());
  }
  return *this;
}

FloatFields &FloatFields::setSNaN() {
  switch(getType()) {
  case FT_FLOAT   : *this = numeric_limits<float   >::signaling_NaN(); break;
  case FT_DOUBLE  : *this = numeric_limits<double  >::signaling_NaN(); break;
  case FT_DOUBLE80: *this = numeric_limits<Double80>::signaling_NaN(); break;
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
  case FT_FLOAT   : return numeric_limits<float   >::max_digits10;
  case FT_DOUBLE  : return numeric_limits<double  >::max_digits10;
  case FT_DOUBLE80: return numeric_limits<Double80>::max_digits10;
  DEFAULT_WRONGTYPE(type);
  }
  return 0;
}

String dataToBinString(const void *data, size_t n) {
  String result;
  const BYTE *p0 = (BYTE*)data, *pp = p0 + (n - 1) / 8;
  for(UINT i = (UINT)n; i--;) {
    if(((i%4) == 3) && !result.isEmpty()) {
      result += ' ';
    }
    result += ((*pp >> (i%8)) & 1) ? '1' : '0';
    if((i%8) == 0) pp--;
  }
  return result;
}

String dataToHexString(const void *data, size_t n) {
  String result;
  BYTE *pp = ((BYTE*)data) + n - 1;
  for(size_t i = 0; i < n; i++) {
    result += format(_T("%02X"), *(pp--));
  }
  for(int dotPos = (int)result.length() - 4; dotPos > 0; dotPos -= 4) {
    result.insert(dotPos, _T('.'));
  }
  return result;
}

String FloatFields::toBinString() const {
  const UINT   expo = getExpoField();
  const UINT64 sig  = getSig();
  return format(_T("Sign:%c   Expo:%-19s   Sig:%s")
               ,getSignBit() ? _T('1') : '0'
               ,dataToBinString(&expo, getExpoBitCount()).cstr()
               ,dataToBinString(&sig , getSigBitCount()).cstr());
}

String FloatFields::getDisplayString() const {
  const int prec = getMaxDigits10();
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
  case FT_FLOAT   : return _fpclassf(getFloat());
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
  case FT_FLOAT   : return FloatFields((float)sqrtf(ff.getFloat()));
  case FT_DOUBLE  : return FloatFields(sqrt(ff.getDouble()));
  case FT_DOUBLE80: return FloatFields(sqrt(ff.getDouble80()));
  DEFAULT_WRONGTYPE(ff.getType());
  }
  return ff;
}

template<class OSTREAMTYPE> OSTREAMTYPE &putff(OSTREAMTYPE &s, const FloatFields &ff) {
  switch(ff.getType()) {
  case FT_FLOAT   : s << ff.getFloat();    break;
  case FT_DOUBLE  : s << ff.getDouble();   break;
  case FT_DOUBLE80: s << ff.getDouble80(); break;
  }
  return s;
}

template<class ISTREAMTYPE> ISTREAMTYPE &getff(ISTREAMTYPE&s, FloatFields &ff) {
  switch(ff.getType()) {
  case FT_FLOAT   : { float    v; s >> v; ff = v; break; }
  case FT_DOUBLE  : { double   v; s >> v; ff = v; break; }
  case FT_DOUBLE80: { Double80 v; s >> v; ff = v; break; }
  }
  return s;
}

ostream &operator<<(ostream &s, const FloatFields &ff) {
  return putff(s, ff);
}

wostream &operator<<(wostream &s, const FloatFields &ff) {
  return putff(s, ff);
}

istream &operator>>(istream &s, FloatFields &ff) {
  return getff(s, ff);
}

wistream &operator>>(wistream &s, FloatFields &ff) {
  return getff(s, ff);
}

istream &FloatFields::input(istream &in, bool manip) {
  float    f;
  double   d;
  Double80 d80;
  if(manip) {
    switch(getType()) {
    case FT_FLOAT   : in >> CharManip<float >   >> f  ; break;
    case FT_DOUBLE  : in >> CharManip<double>   >> d  ; break;
    case FT_DOUBLE80: in >> CharManip<Double80> >> d80; break;
    }
  } else {
    switch(getType()) {
    case FT_FLOAT   : in >> f  ; break;
    case FT_DOUBLE  : in >> d  ; break;
    case FT_DOUBLE80: in >> d80; break;
    }
  }
  switch(getType()) {
  case FT_FLOAT   : *this = f  ; break;
  case FT_DOUBLE  : *this = d  ; break;
  case FT_DOUBLE80: *this = d80; break;
  }
  return in;
}

wistream &FloatFields::input(wistream &in, bool manip) {
  float    f;
  double   d;
  Double80 d80;
  if(manip) {
    switch(getType()) {
    case FT_FLOAT   : in >> WcharManip<float >   >> f  ; break;
    case FT_DOUBLE  : in >> WcharManip<double>   >> d  ; break;
    case FT_DOUBLE80: in >> WcharManip<Double80> >> d80; break;
    }
  } else {
    switch(getType()) {
    case FT_FLOAT   : in >> f  ; break;
    case FT_DOUBLE  : in >> d  ; break;
    case FT_DOUBLE80: in >> d80; break;
    }
  }
  switch(getType()) {
  case FT_FLOAT   : *this = f  ; break;
  case FT_DOUBLE  : *this = d  ; break;
  case FT_DOUBLE80: *this = d80; break;
  }
  return in;
}
