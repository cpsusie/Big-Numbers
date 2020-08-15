#include "pch.h"
#include <Math/Number.h>
#include <Math/Double64.h>
#include <Math/Double80.h>

using namespace std;

// fpclass must be one of:
// _FPCLASS_SNAN  0x0001   signaling NaN
// _FPCLASS_QNAN  0x0002   quiet NaN
// _FPCLASS_NINF  0x0004   negative infinity
// _FPCLASS_PINF  0x0200   positive infinity
void Number::setToNaN(int fpclass, bool init) {
  setType(NUMBERTYPE_FLOAT, init);
  switch(fpclass) {
  case _FPCLASS_SNAN: *m_flt =  std::numeric_limits<float>::signaling_NaN(); break;
  case _FPCLASS_QNAN: *m_flt =  std::numeric_limits<float>::quiet_NaN();     break;
  case _FPCLASS_NINF: *m_flt = -std::numeric_limits<float>::infinity();      break;
  case _FPCLASS_PINF: *m_flt =  std::numeric_limits<float>::infinity();      break;
  default           : throwInvalidArgumentException(__TFUNCTION__, _T("fpclass=%d"), fpclass);
  }
}

Number::Number(const Number &v) {
  if(!isfinite(v)) {
    initToNaN(_fpclass(v));
  } else {
    initType(v.getType());
    switch(m_type) {
    case NUMBERTYPE_FLOAT    : *m_flt      = *v.m_flt;      break;
    case NUMBERTYPE_DOUBLE   : *m_d64      = *v.m_d64;      break;
    case NUMBERTYPE_DOUBLE80 : *m_d80      = *v.m_d80;      break;
    case NUMBERTYPE_RATIONAL : *m_rational = *v.m_rational; break;
    }
  }
}

Number::Number(const float &v) {
  if(handleInfAndRationalValue(v,true)) {
    return;
  }
  initType(NUMBERTYPE_FLOAT);
  *m_flt = v;
}

Number::Number(const double &v) {
  if(handleInfAndRationalValue(v, true)) {
    return;
  }
  initType(NUMBERTYPE_DOUBLE);
  *m_d64 = v;
}

Number::Number(const Double80 &v) {
  if(handleInfAndRationalValue(v, true)) {
    return;
  }
  initType(NUMBERTYPE_DOUBLE80);
  *m_d80 = v;
}

Number::Number(const Rational &v) {
  if(!isfinite(v)) {
    initToNaN(_fpclass(v));
  } else {
    initType(NUMBERTYPE_RATIONAL);
    *m_rational = v;
  }
}

Number::Number(int v) {
  initType(NUMBERTYPE_RATIONAL);
  *m_rational = v;
}

Number::Number(UINT v) {
  initType(NUMBERTYPE_RATIONAL);
  *m_rational = v;
}

int _fpclass(const Number &n) {
  switch(n.getType()) {
  case NUMBERTYPE_FLOAT   : return ::_fpclass((float   )n);
  case NUMBERTYPE_DOUBLE  : return ::_fpclass((double  )n);
  case NUMBERTYPE_DOUBLE80: return ::_fpclass((Double80)n);
  case NUMBERTYPE_RATIONAL: return ::_fpclass((Rational)n);
  default                 : n.throwUnknownTypeException(__TFUNCTION__);
  }
  return 0;
}

int fpclassify(const Number &n) {
  switch(n.getType()) {
  case NUMBERTYPE_FLOAT   : return fpclassify((float   )n);
  case NUMBERTYPE_DOUBLE  : return fpclassify((double  )n);
  case NUMBERTYPE_DOUBLE80: return fpclassify((Double80)n);
  case NUMBERTYPE_RATIONAL: return fpclassify((Rational)n);
  default                 : n.throwUnknownTypeException(__TFUNCTION__);
  }
  return 0;
}

void Number::setType(NumberType type, bool init) {
  if(!init) {
    if(type == m_type) {
      return;
    }
    cleanup();
  }
  switch(type) {
  case NUMBERTYPE_FLOAT    : m_flt      = new float;    TRACE_NEW(m_flt     ); break;
  case NUMBERTYPE_DOUBLE   : m_d64      = new double;   TRACE_NEW(m_d64     ); break;
  case NUMBERTYPE_DOUBLE80 : m_d80      = new Double80; TRACE_NEW(m_d80     ); break;
  case NUMBERTYPE_RATIONAL : m_rational = new Rational; TRACE_NEW(m_rational); break;
  default                  : throwUnknownTypeException(__TFUNCTION__, type);
  }
  m_type = type;
}

void Number::cleanup() {
  switch(m_type) {
  case NUMBERTYPE_FLOAT    : SAFEDELETE(m_flt     ); break;
  case NUMBERTYPE_DOUBLE   : SAFEDELETE(m_d64     ); break;
  case NUMBERTYPE_DOUBLE80 : SAFEDELETE(m_d80     ); break;
  case NUMBERTYPE_RATIONAL : SAFEDELETE(m_rational); break;
  default                  : throwUnknownTypeException(__TFUNCTION__);
  }
}

String Number::getTypeName(NumberType nt) { // static
#define CASESTR(t) case NUMBERTYPE_##t: return _T(#t);
  switch(nt) {
  CASESTR(FLOAT    );
  CASESTR(DOUBLE   );
  CASESTR(DOUBLE80 );
  CASESTR(RATIONAL );
  default:return format(_T("Unknown numbertype:%d"), nt);
  }
}

void Number::throwUnknownTypeException(const TCHAR *method) const {
  throwUnknownTypeException(method, m_type);
}

void Number::throwUnknownTypeException(const TCHAR *method, NumberType type) { // static
  throwException(_T("%s:Unknown type:%d"), method, type);
}

Number &Number::operator=(const Number &v) {
  if(&v == this) {
    return *this;
  }
  if(!isfinite(v)) {
    setToNaN(_fpclass(v));
    return *this;
  }
  if(m_type != v.m_type) {
    setType(v.m_type);
  }
  switch(m_type) {
  case NUMBERTYPE_FLOAT    : *m_flt      = *v.m_flt     ; break;
  case NUMBERTYPE_DOUBLE   : *m_d64      = *v.m_d64     ; break;
  case NUMBERTYPE_DOUBLE80 : *m_d80      = *v.m_d80     ; break;
  case NUMBERTYPE_RATIONAL : *m_rational = *v.m_rational; break;
  default                  : v.throwUnknownTypeException(__TFUNCTION__);
  }
  return *this;
}

Number &Number::operator=(const float &v) {
  if(!handleInfAndRationalValue(v)) {
    setType(NUMBERTYPE_FLOAT);
    *m_flt = v;
  }
  return *this;
}

Number &Number::operator=(const double &v) {
  if(!handleInfAndRationalValue(v)) {
    setType(NUMBERTYPE_DOUBLE);
    *m_d64 = v;
  }
  return *this;
}

Number &Number::operator=(const Double80 &v) {
  if(!handleInfAndRationalValue(v)) {
    setType(NUMBERTYPE_DOUBLE80);
    *m_d80 = v;
  }
  return *this;
}

Number &Number::operator=(const Rational &v) {
  if(!isfinite(v)) {
    setToNaN(_fpclass(v));
    return *this;
  }
  setType(NUMBERTYPE_RATIONAL);
  *m_rational = v;
  return *this;
}

Number &Number::operator=(int v) {
  setType(NUMBERTYPE_RATIONAL);
  *m_rational = v;
  return *this;
}

Number &Number::operator=(UINT v) {
  setType(NUMBERTYPE_RATIONAL);
  *m_rational = v;
  return *this;
}

Number::operator int()      const {
  switch(getType()) {
  case NUMBERTYPE_FLOAT    : return (int)*m_flt;
  case NUMBERTYPE_DOUBLE   : return (int)*m_d64;
  case NUMBERTYPE_DOUBLE80 : return (int)*m_d80;
  case NUMBERTYPE_RATIONAL : return (int)*m_rational;
  default                  : throwUnknownTypeException(__TFUNCTION__);
  }
  return 0;
}

Number::operator float()    const {
  switch(getType()) {
  case NUMBERTYPE_FLOAT    : return *m_flt;
  case NUMBERTYPE_DOUBLE   : return (float)*m_d64;
  case NUMBERTYPE_DOUBLE80 : return (float)*m_d80;
  case NUMBERTYPE_RATIONAL : return (float)*m_rational;
  default                  : throwUnknownTypeException(__TFUNCTION__);
  }
  return 0;
}

Number::operator double()   const {
  switch(getType()) {
  case NUMBERTYPE_FLOAT    : return *m_flt;
  case NUMBERTYPE_DOUBLE   : return *m_d64;
  case NUMBERTYPE_DOUBLE80 : return (double)*m_d80;
  case NUMBERTYPE_RATIONAL : return (double)*m_rational;
  default                  : throwUnknownTypeException(__TFUNCTION__);
  }
  return 0;
}

Number::operator Double80() const {
  switch(getType()) {
  case NUMBERTYPE_FLOAT    : return *m_flt;
  case NUMBERTYPE_DOUBLE   : return *m_d64;
  case NUMBERTYPE_DOUBLE80 : return *m_d80;
  case NUMBERTYPE_RATIONAL : return (Double80)*m_rational;
  default                  : throwUnknownTypeException(__TFUNCTION__);
  }
  return 0;
}

Number::operator Rational() const {
  switch(getType()) {
  case NUMBERTYPE_FLOAT    : return Rational(*m_flt);
  case NUMBERTYPE_DOUBLE   : return Rational(*m_d64);
  case NUMBERTYPE_DOUBLE80 : return Rational(*m_d80);
  case NUMBERTYPE_RATIONAL : return *m_rational;
  default                  : throwUnknownTypeException(__TFUNCTION__);
  }
  return 0;
}

Number operator+(const Number &n1, const Number &n2) {
  const NumberType t1 = n1.getType(), t2 = n2.getType();
  if(t1 == t2) {
    switch(t1) {
    case NUMBERTYPE_FLOAT   :
      { const float r = *n1.m_flt + *n2.m_flt;
        if(isfinite(r) || !isfinite(n1) || !isfinite(n2)) return r;
      }
      break;
    case NUMBERTYPE_DOUBLE  :
      { const double r = *n1.m_d64 + *n2.m_d64;
        if(isfinite(r) || !isfinite(n1) || !isfinite(n2)) return r;
      }
      break;
    case NUMBERTYPE_DOUBLE80:
      break;
    case NUMBERTYPE_RATIONAL:
      try {
        return *n1.m_rational + *n2.m_rational;
      } catch(Exception) {
        break;
      }
    default                 :
      Number::throwUnknownTypeException(__TFUNCTION__, t1);
    }
  }
  return (Double80)n1 + (Double80)n2;
}

Number operator-(const Number &n1, const Number &n2) {
  const NumberType t1 = n1.getType(), t2 = n2.getType();
  if(t1 == t2) {
    switch(t1) {
    case NUMBERTYPE_FLOAT   :
      { const float r = *n1.m_flt - *n2.m_flt;
        if(isfinite(r) || !isfinite(n1) || !isfinite(n2)) return r;
      }
      break;
    case NUMBERTYPE_DOUBLE  :
      { const double r = *n1.m_d64 - *n2.m_d64;
        if(isfinite(r) || !isfinite(n1) || !isfinite(n2)) return r;
      }
      break;
    case NUMBERTYPE_DOUBLE80:
      break;
    case NUMBERTYPE_RATIONAL:
      try {
        return *n1.m_rational - *n2.m_rational;
      } catch(Exception) {
        break;
      }
    default                 :
      Number::throwUnknownTypeException(__TFUNCTION__, t1);
    }
  }
  return (Double80)n1 - (Double80)n2;
}

Number operator-(const Number &v) {
  switch(v.getType()) {
  case NUMBERTYPE_FLOAT    : return -*v.m_flt;
  case NUMBERTYPE_DOUBLE   : return -*v.m_d64;
  case NUMBERTYPE_DOUBLE80 : return -*v.m_d80;
  case NUMBERTYPE_RATIONAL : return -*v.m_rational;
  default                  : Number::throwUnknownTypeException(__TFUNCTION__,v.getType());
  }
  return 0;
}

Number operator*(const Number &n1, const Number &n2) {
  const NumberType t1 = n1.getType(), t2 = n2.getType();
  if(t1 == t2) {
    switch(t1) {
    case NUMBERTYPE_FLOAT   :
      { const float r = *n1.m_flt * *n2.m_flt;
        if(isfinite(r) || !isfinite(n1) || !isfinite(n2)) return r;
      }
      break;
    case NUMBERTYPE_DOUBLE  :
      { const double r = *n1.m_d64 * *n2.m_d64;
        if(isfinite(r) || !isfinite(n1) || !isfinite(n2)) return r;
      }
      break;
    case NUMBERTYPE_DOUBLE80:
      break;
    case NUMBERTYPE_RATIONAL:
      try {
        return *n1.m_rational * *n2.m_rational;
      } catch(Exception) {
        break;
      }
    default                 :
      Number::throwUnknownTypeException(__TFUNCTION__, t1);
    }
  }
  return (Double80)n1 * (Double80)n2;
}

Number operator/(const Number &n1, const Number &n2) {
  const NumberType t1 = n1.getType(), t2 = n2.getType();
  if(t1 == t2) {
    switch(t1) {
    case NUMBERTYPE_FLOAT   :
      { const float r = *n1.m_flt / *n2.m_flt;
        if(isfinite(r) || !isfinite(n1) || !isfinite(n2)) return r;
      }
      break;
    case NUMBERTYPE_DOUBLE  :
      { const double r = *n1.m_d64 / *n2.m_d64;
        if(isfinite(r) || !isfinite(n1) || !isfinite(n2)) return r;
      }
      break;
    case NUMBERTYPE_DOUBLE80:
      break;
    case NUMBERTYPE_RATIONAL:
      try {
        return *n1.m_rational / *n2.m_rational;
      } catch(Exception) {
        break;
      }
    default                 :
      Number::throwUnknownTypeException(__TFUNCTION__, t1);
    }
  }
  return (Double80)n1 / (Double80)n2;
}

Number operator%(const Number &n1, const Number &n2) {
  const NumberType t1 = n1.getType(), t2 = n2.getType();
  if(t1 == t2) {
    switch(t1) {
    case NUMBERTYPE_FLOAT   : return Number(fmod(*n1.m_flt, *n2.m_flt)     );
    case NUMBERTYPE_DOUBLE  : return Number(fmod(*n1.m_d64, *n2.m_d64)     );
    case NUMBERTYPE_DOUBLE80: return Number(fmod(*n1.m_d80,*n2.m_d80 )     );
    case NUMBERTYPE_RATIONAL: return Number(*n1.m_rational % *n2.m_rational);
    default                 : Number::throwUnknownTypeException(__TFUNCTION__, t1);
    }
  }
  return Number(fmod((Double80)n1, (Double80)n2));
}

Number reciprocal(const Number &n) {
  switch(n.getType()) {
  case NUMBERTYPE_FLOAT   : return Number(1.0f         / (float)   n);
  case NUMBERTYPE_DOUBLE  : return Number(1.0          / (double)  n);
  case NUMBERTYPE_DOUBLE80: return Number(Double80::_1 / (Double80)n);
  case NUMBERTYPE_RATIONAL: return Number(reciprocal((Rational)n));
  default                 : Number::throwUnknownTypeException(__TFUNCTION__, n.getType());
  }
  return 0;
}

Number pow(const Number &n1, const Number &n2) {
  if(n2.getType() == NUMBERTYPE_RATIONAL) {
    const Rational r2 = (Rational)n2;
    if(n1.getType() == NUMBERTYPE_RATIONAL) {
      Rational result;
      if(Rational::isRationalPow((Rational)n1, r2, &result)) {
        return result;
      }
    }
    return Rational::pow((Double80)n1, r2);
  }
  return mypow((Double80)n1, (Double80)n2);
}

int numberCmp(const Number &n1, const Number &n2) {
  if(&n1 == &n2) return true;
  const NumberType t1 = n1.getType(), t2 = n2.getType();
  if(t1 == t2) {
    switch(t1) {
    case NUMBERTYPE_FLOAT   : return sign(*n1.m_flt - *n2.m_flt);
    case NUMBERTYPE_DOUBLE  : return sign(*n1.m_d64 - *n2.m_d64);
    case NUMBERTYPE_DOUBLE80: return sign(*n1.m_d80 - *n2.m_d80);
    case NUMBERTYPE_RATIONAL: return rationalCmp(*n1.m_rational, *n2.m_rational);
    default                 : Number::throwUnknownTypeException(__TFUNCTION__, t1);
    }
  }
  return sign((Double80)n1 - (Double80)n2);
}
