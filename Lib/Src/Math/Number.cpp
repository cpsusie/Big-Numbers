#include "pch.h"
#include <Math/Number.h>
#include <Math/Double64.h>

Number::Number() {
  m_type     = NUMBERTYPE_UNDEFINED;
  m_rational = NULL;
}

Number::Number(const Number &v) {
  m_type = v.m_type;
  switch(m_type) {
  case NUMBERTYPE_UNDEFINED: m_rational = NULL;                                               break;
  case NUMBERTYPE_FLOAT    : m_flt      = new float(*v.m_flt);         TRACE_NEW(m_flt);      break;
  case NUMBERTYPE_DOUBLE   : m_d64      = new double(*v.m_d64);        TRACE_NEW(m_d64);      break;
  case NUMBERTYPE_DOUBLE80 : m_d80      = new Double80(*v.m_d80);      TRACE_NEW(m_d80);      break;
  case NUMBERTYPE_RATIONAL : m_rational = new Rational(*v.m_rational); TRACE_NEW(m_rational); break;
  default                  : throwUnknownTypeException(__TFUNCTION__);
  }
}

Number::Number(const float &v) {
  Rational tmp;
  if(Rational::isRational(v, &tmp)) {
    m_type     = NUMBERTYPE_RATIONAL;
    m_rational = new Rational(tmp); TRACE_NEW(m_rational);
  } else if(isnan(v)) {
    m_type     = NUMBERTYPE_UNDEFINED;
    m_rational = NULL;
  } else {
    m_type     = NUMBERTYPE_FLOAT;
    m_flt      = new float(v);      TRACE_NEW(m_flt);
  }
}

Number::Number(const double &v) {
  Rational tmp;
  if(Rational::isRational(v, &tmp)) {
    m_type     = NUMBERTYPE_RATIONAL;
    m_rational = new Rational(tmp); TRACE_NEW(m_rational);
  } else if(isnan(v)) {
    m_type     = NUMBERTYPE_UNDEFINED;
    m_rational = NULL;
  } else {
    m_type     = NUMBERTYPE_DOUBLE;
    m_d64      = new double(v);     TRACE_NEW(m_d64);
  }
}

Number::Number(const Double80 &v) {
  Rational tmp;
  if(Rational::isRational(v, &tmp)) {
    m_type     = NUMBERTYPE_RATIONAL;
    m_rational = new Rational(tmp); TRACE_NEW(m_rational);
  } else if(isnan(v)) {
    m_type     = NUMBERTYPE_UNDEFINED;
    m_rational = NULL;
  } else {
    m_type     = NUMBERTYPE_DOUBLE80;
    m_d80      = new Double80(v);   TRACE_NEW(m_d80);
  }
}

Number::Number(const Rational &v) {
  m_type     = NUMBERTYPE_RATIONAL;
  m_rational = new Rational(v);     TRACE_NEW(m_rational);
}

Number::Number(int v) {
  m_type     = NUMBERTYPE_RATIONAL;
  m_rational = new Rational(v);     TRACE_NEW(m_rational);
}

Number::Number(UINT v) {
  m_type     = NUMBERTYPE_RATIONAL;
  m_rational = new Rational(v);     TRACE_NEW(m_rational);
}

Number::~Number() {
  cleanup();
}

void Number::setType(NumberType type) {
  DEFINEMETHODNAME;
  if(type == m_type) return;
  cleanup();
  switch(type) {
  case NUMBERTYPE_UNDEFINED: return;
  case NUMBERTYPE_FLOAT    : m_flt      = new float;    TRACE_NEW(m_flt     ); break;
  case NUMBERTYPE_DOUBLE   : m_d64      = new double;   TRACE_NEW(m_d64     ); break;
  case NUMBERTYPE_DOUBLE80 : m_d80      = new Double80; TRACE_NEW(m_d80     ); break;
  case NUMBERTYPE_RATIONAL : m_rational = new Rational; TRACE_NEW(m_rational); break;
  default                  : throwUnknownTypeException(method, type);
  }
  m_type = type;
}

void Number::cleanup() {
  switch(m_type) {
  case NUMBERTYPE_UNDEFINED: return;
  case NUMBERTYPE_FLOAT    : SAFEDELETE(m_flt     ); break;
  case NUMBERTYPE_DOUBLE   : SAFEDELETE(m_d64     ); break;
  case NUMBERTYPE_DOUBLE80 : SAFEDELETE(m_d80     ); break;
  case NUMBERTYPE_RATIONAL : SAFEDELETE(m_rational); break;
  default                  : throwUnknownTypeException(__TFUNCTION__);
  }
  m_type = NUMBERTYPE_UNDEFINED;
}

String Number::getTypeName(NumberType nt) { // static
#define CASESTR(t) case NUMBERTYPE_##t: return _T(#t);
  switch(nt) {
  CASESTR(UNDEFINED);
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

void Number::throwTypeIsUndefinedException(const TCHAR *method) { // static
  throwException(_T("%s:Number is undefined"), method);
}

Number &Number::operator=(const Number &v) {
  if(&v == this) {
    return *this;
  }
  if(m_type != v.m_type) {
    setType(v.m_type);
  }
  switch(m_type) {
  case NUMBERTYPE_UNDEFINED: break;
  case NUMBERTYPE_FLOAT    : *m_flt      = *v.m_flt     ; break;
  case NUMBERTYPE_DOUBLE   : *m_d64      = *v.m_d64     ; break;
  case NUMBERTYPE_DOUBLE80 : *m_d80      = *v.m_d80     ; break;
  case NUMBERTYPE_RATIONAL : *m_rational = *v.m_rational; break;
  default                  : v.throwUnknownTypeException(__TFUNCTION__);
  }
  return *this;
}

Number &Number::operator=(const float &v) {
  Rational tmp;
  if(Rational::isRational(v, &tmp)) {
    if(m_type != NUMBERTYPE_RATIONAL) {
      setType(NUMBERTYPE_RATIONAL);
    }
    *m_rational = tmp;
  } else if(isnan(v)) {
    setType(NUMBERTYPE_UNDEFINED);
  } else {
    setType(NUMBERTYPE_FLOAT);
    *m_flt = v;
  }
  return *this;
}

Number &Number::operator=(const double &v) {
  Rational tmp;
  if(Rational::isRational(v, &tmp)) {
    if(m_type != NUMBERTYPE_RATIONAL) {
      setType(NUMBERTYPE_RATIONAL);
    }
    *m_rational = tmp;
  } else if(isnan(v)) {
    setType(NUMBERTYPE_UNDEFINED);
  } else {
    setType(NUMBERTYPE_DOUBLE);
    *m_d64 = v;
  }
  return *this;
}

Number &Number::operator=(const Double80 &v) {
  Rational tmp;
  if(Rational::isRational(v, &tmp)) {
    if(m_type != NUMBERTYPE_RATIONAL) {
      setType(NUMBERTYPE_RATIONAL);
    }
    *m_rational = tmp;
  } else if(isnan(v)) {
    setType(NUMBERTYPE_UNDEFINED);
  } else {
    setType(NUMBERTYPE_DOUBLE80);
    *m_d80 = v;
  }
  return *this;
}

Number &Number::operator=(const Rational &v) {
  if(m_type != NUMBERTYPE_RATIONAL) {
    setType(NUMBERTYPE_RATIONAL);
  }
  *m_rational = v;
  return *this;
}

Number &Number::operator=(int v) {
  if(m_type != NUMBERTYPE_RATIONAL) {
    setType(NUMBERTYPE_RATIONAL);
  }
  *m_rational = v;
  return *this;
}

Number &Number::operator=(UINT v) {
  if(m_type != NUMBERTYPE_RATIONAL) {
    setType(NUMBERTYPE_RATIONAL);
  }
  *m_rational = v;
  return *this;
}

int getInt(const Number &n) {
  DEFINEMETHODNAME;
  switch(n.getType()) {
  case NUMBERTYPE_UNDEFINED: Number::throwTypeIsUndefinedException(method);
  case NUMBERTYPE_FLOAT    : return getInt(*n.m_flt     );
  case NUMBERTYPE_DOUBLE   : return getInt(*n.m_d64     );
  case NUMBERTYPE_DOUBLE80 : return getInt(*n.m_d80     );
  case NUMBERTYPE_RATIONAL : return getInt(*n.m_rational);
  default                  : n.throwUnknownTypeException(method);
  }
  return 0;
}

float getFloat(const Number &n) {
  DEFINEMETHODNAME;
  switch(n.getType()) {
  case NUMBERTYPE_UNDEFINED: Number::throwTypeIsUndefinedException(method);
  case NUMBERTYPE_FLOAT    : return *n.m_flt;
  case NUMBERTYPE_DOUBLE   : return getFloat(*n.m_d64);
  case NUMBERTYPE_DOUBLE80 : return getFloat(*n.m_d80);
  case NUMBERTYPE_RATIONAL : return getFloat(*n.m_rational);
  default                  : n.throwUnknownTypeException(method);
  }
  return 0;
}

double getDouble(const Number &n) {
  DEFINEMETHODNAME;
  switch(n.getType()) {
  case NUMBERTYPE_UNDEFINED: Number::throwTypeIsUndefinedException(method);
  case NUMBERTYPE_FLOAT    : return *n.m_flt;
  case NUMBERTYPE_DOUBLE   : return *n.m_d64;
  case NUMBERTYPE_DOUBLE80 : return getDouble(*n.m_d80);
  case NUMBERTYPE_RATIONAL : return getDouble(*n.m_rational);
  default                  : n.throwUnknownTypeException(method);
  }
  return 0;
}

Double80 getDouble80(const Number &n) {
  DEFINEMETHODNAME;
  switch(n.getType()) {
  case NUMBERTYPE_UNDEFINED: Number::throwTypeIsUndefinedException(method);
  case NUMBERTYPE_FLOAT    : return *n.m_flt;
  case NUMBERTYPE_DOUBLE   : return *n.m_d64;
  case NUMBERTYPE_DOUBLE80 : return *n.m_d80;
  case NUMBERTYPE_RATIONAL : return getDouble80(*n.m_rational);
  default                  : n.throwUnknownTypeException(method);
  }
  return 0;
}

Rational getRational(const Number &n) {
  DEFINEMETHODNAME;
  switch(n.getType()) {
  case NUMBERTYPE_UNDEFINED: Number::throwTypeIsUndefinedException(method);
  case NUMBERTYPE_FLOAT    : return Rational(*n.m_flt);
  case NUMBERTYPE_DOUBLE   : return Rational(*n.m_d64);
  case NUMBERTYPE_DOUBLE80 : return Rational(*n.m_d80);
  case NUMBERTYPE_RATIONAL : return *n.m_rational;
  default                  : n.throwUnknownTypeException(method);
  }
  return 0;
}

String Number::toString() const {
  switch(getType()) {
  case NUMBERTYPE_UNDEFINED: return _T("undefined");
  case NUMBERTYPE_FLOAT    : return ::toString(*m_flt     );
  case NUMBERTYPE_DOUBLE   : return ::toString(*m_d64     );
  case NUMBERTYPE_DOUBLE80 : return ::toString(*m_d80     );
  case NUMBERTYPE_RATIONAL : return ::toString(*m_rational);
  default                  : return format(_T("%s:Unknown type:%d"), __TFUNCTION__, getType());
  }
}

Number operator+(const Number &n1, const Number &n2) {
  if(isRational(n1) && isRational(n2)) {
    return *n1.m_rational + *n2.m_rational;
  } else {
    return getReal(n1) + getReal(n2);
  }
}

Number operator-(const Number &n1, const Number &n2) {
  if(isRational(n1) && isRational(n2)) {
    return *n1.m_rational - *n2.m_rational;
  } else {
    return getReal(n1) - getReal(n2);
  }
}

Number operator-(const Number &v) {
  DEFINEMETHODNAME;
  switch(v.getType()) {
  case NUMBERTYPE_UNDEFINED: Number::throwTypeIsUndefinedException(method);
  case NUMBERTYPE_FLOAT    : return -*v.m_flt;
  case NUMBERTYPE_DOUBLE   : return -*v.m_d64;
  case NUMBERTYPE_DOUBLE80 : return -*v.m_d80;
  case NUMBERTYPE_RATIONAL : return -getRational(v);
  default                  : v.throwUnknownTypeException(method);
  }
  return 0;
}

Number operator*(const Number &n1, const Number &n2) {
  if(isRational(n1) && isRational(n2)) {
    return *n1.m_rational * *n2.m_rational;
  } else {
    return getReal(n1) * getReal(n2);
  }
}

Number operator/(const Number &n1, const Number &n2) {
  if(isRational(n1) && isRational(n2)) {
    return *n1.m_rational / *n2.m_rational;
  } else {
    return getReal(n1) / getReal(n2);
  }
}

Number operator%(const Number &n1, const Number &n2) {
  if(isRational(n1) && isRational(n2)) {
    return *n1.m_rational % *n2.m_rational;
  } else {
    const Real r = fmod(getReal(n1), getReal(n2));
    return Number(r);
  }
}

Number reciprocal(const Number &n) {
  if(isRational(n)) {
    return reciprocal(*n.m_rational);
  } else {
    return 1.0/getReal(n);
  }
}

Number pow(const Number &n1, const Number &n2) {
  if(isRational(n1) && isInt(n2)) {
    return pow(*n1.m_rational, getInt(n2));
  } else {
    const Real r = mypow(getReal(n1), getReal(n2));
    return Number(r);
  }
}

int numberCmp(const Number &n1, const Number &n2) {
  DEFINEMETHODNAME;
  const NumberType t1 = n1.getType();
  const NumberType t2 = n2.getType();
  if(t1 == NUMBERTYPE_UNDEFINED) {
    throwInvalidArgumentException(method, _T("n1 is undefined"));
  } else if(t2 == NUMBERTYPE_UNDEFINED) {
    throwInvalidArgumentException(method, _T("n2 is undefined"));
  }
  if((t1 == NUMBERTYPE_RATIONAL) && (t2 == NUMBERTYPE_RATIONAL)) {
    return rationalCmp(getRational(n1), getRational(n2));
  } else {
    return sign(getReal(n1) - getReal(n2));
  }
}

bool Number::operator==(const Number &n) const {
  DEFINEMETHODNAME;
  if(&n == this) return true;
  const NumberType type = getType();
  if(type != n.getType()) return false;
  switch(type) {
  case NUMBERTYPE_UNDEFINED: throwTypeIsUndefinedException(method);
  case NUMBERTYPE_FLOAT    : return *m_flt      == *n.m_flt;
  case NUMBERTYPE_DOUBLE   : return *m_d64      == *n.m_d64;
  case NUMBERTYPE_DOUBLE80 : return *m_d80      == *n.m_d80;
  case NUMBERTYPE_RATIONAL : return *m_rational == *n.m_rational;
  default                  : throwUnknownTypeException(method);
  }
  return false;
}
