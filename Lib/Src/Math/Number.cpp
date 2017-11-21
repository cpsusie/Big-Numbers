#include "pch.h"
#include <Math/MathFunctions.h>
#include <Math/Number.h>

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
  } else if(isNan(v)) {
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
  } else if(isNan(v)) {
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
  } else if(isNan(v)) {
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

Number::Number(const wchar_t *s) {
  USES_WCONVERSION;
  init(WSTR2TSTR(s));
}

Number::Number(const char *s) {
  USES_ACONVERSION;
  init(ASTR2TSTR(s));
}

void Number::init(const _TUCHAR *s) {
  m_type     = NUMBERTYPE_UNDEFINED;
  m_rational = NULL;
  const _TUCHAR *s1 = parseReal(    s);
  const _TUCHAR *s2 = parseRational(s);
  if(s1 == NULL) {
    if(s2 == NULL) {
      setType(NUMBERTYPE_UNDEFINED);
    } else {
      *this = Rational(s);
    }
  } else if(s2 == NULL) { // s1 != NULL
    *this = Double80(s);
  } else { // s1 != NULL && s2 != NULL
    if(s2 > s1) { // must be rational
      *this = Rational(s);
    } else {
      *this = Double80(s);
    }
  }
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
  } else if(isNan(v)) {
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
  } else if(isNan(v)) {
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
  } else if(isNan(v)) {
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

int Number::getIntValue() const {
  DEFINEMETHODNAME;
  switch(getType()) {
  case NUMBERTYPE_UNDEFINED: throwTypeIsUndefinedException(method);
  case NUMBERTYPE_FLOAT    : return getInt(*m_flt     );
  case NUMBERTYPE_DOUBLE   : return getInt(*m_d64     );
  case NUMBERTYPE_DOUBLE80 : return getInt(*m_d80     );
  case NUMBERTYPE_RATIONAL : return getInt(*m_rational);
  default                  : throwUnknownTypeException(method);
  }
  return 0;
}

float Number::getFloatValue() const {
  DEFINEMETHODNAME;
  switch(getType()) {
  case NUMBERTYPE_UNDEFINED: throwTypeIsUndefinedException(method);
  case NUMBERTYPE_FLOAT    : return *m_flt;
  case NUMBERTYPE_DOUBLE   : return getFloat(*m_d64);
  case NUMBERTYPE_DOUBLE80 : return getFloat(*m_d80);
  case NUMBERTYPE_RATIONAL : return getFloat(*m_rational);
  default                  : throwUnknownTypeException(method);
  }
  return 0;
}

double Number::getDoubleValue() const {
  DEFINEMETHODNAME;
  switch(getType()) {
  case NUMBERTYPE_UNDEFINED: throwTypeIsUndefinedException(method);
  case NUMBERTYPE_FLOAT    : return *m_flt;
  case NUMBERTYPE_DOUBLE   : return *m_d64;
  case NUMBERTYPE_DOUBLE80 : return getDouble(*m_d80);
  case NUMBERTYPE_RATIONAL : return getDouble(*m_rational);
  default                  : throwUnknownTypeException(method);
  }
  return 0;
}

Double80 Number::getDouble80Value() const {
  DEFINEMETHODNAME;
  switch(getType()) {
  case NUMBERTYPE_UNDEFINED: throwTypeIsUndefinedException(method);
  case NUMBERTYPE_FLOAT    : return *m_flt;
  case NUMBERTYPE_DOUBLE   : return *m_d64;
  case NUMBERTYPE_DOUBLE80 : return *m_d80;
  case NUMBERTYPE_RATIONAL : return getDouble80(*m_rational);
  default                  : throwUnknownTypeException(method);
  }
  return 0;
}

Rational Number::getRationalValue() const {
  DEFINEMETHODNAME;
  switch(getType()) {
  case NUMBERTYPE_UNDEFINED: throwTypeIsUndefinedException(method);
  case NUMBERTYPE_FLOAT    : return Rational(*m_flt);
  case NUMBERTYPE_DOUBLE   : return Rational(*m_d64);
  case NUMBERTYPE_DOUBLE80 : return Rational(*m_d80);
  case NUMBERTYPE_RATIONAL : return *m_rational;
  default                  : throwUnknownTypeException(method);
  }
  return 0;
}

String Number::toString() const {
  switch(getType()) {
  case NUMBERTYPE_UNDEFINED: return _T("undefined");
  case NUMBERTYPE_FLOAT    : return ::toString(*m_flt);
  case NUMBERTYPE_DOUBLE   : return ::toString(*m_d64);
  case NUMBERTYPE_DOUBLE80 : return ::toString(*m_d80);
  case NUMBERTYPE_RATIONAL : return m_rational->toString();
  default                  : return format(_T("%s:Unknown type:%d"), __TFUNCTION__, getType());
  }
}

Number operator+(const Number &n1, const Number &n2) {
  if(n1.isRational() && n2.isRational()) {
    return *n1.m_rational + *n2.m_rational;
  } else {
    return n1.getRealValue() + n2.getRealValue();
  }
}

Number operator-(const Number &n1, const Number &n2) {
  if(n1.isRational() && n2.isRational()) {
    return *n1.m_rational - *n2.m_rational;
  } else {
    return n1.getRealValue() - n2.getRealValue();
  }
}

Number operator-(const Number &v) {
  DEFINEMETHODNAME;
  switch(v.getType()) {
  case NUMBERTYPE_UNDEFINED: Number::throwTypeIsUndefinedException(method);
  case NUMBERTYPE_FLOAT    : return -*v.m_flt;
  case NUMBERTYPE_DOUBLE   : return -*v.m_d64;
  case NUMBERTYPE_DOUBLE80 : return -*v.m_d80;
  case NUMBERTYPE_RATIONAL : return -v.getRationalValue();
  default                  : Number::throwUnknownTypeException(method, v.getType());
  }
  return 0;
}

Number operator*(const Number &n1, const Number &n2) {
  if(n1.isRational() && n2.isRational()) {
    return *n1.m_rational * *n2.m_rational;
  } else {
    return n1.getRealValue() * n2.getRealValue();
  }
}

Number operator/(const Number &n1, const Number &n2) {
  if(n1.isRational() && n2.isRational()) {
    return *n1.m_rational / *n2.m_rational;
  } else {
    return n1.getRealValue() / n2.getRealValue();
  }
}

Number operator%(const Number &n1, const Number &n2) {
  if(n1.isRational() && n2.isRational()) {
    return *n1.m_rational % *n2.m_rational;
  } else {
    const Real r = fmod(n1.getRealValue(), n2.getRealValue());
    return Number(r);
  }
}

Number reciprocal(const Number &n) {
  if(n.isRational()) {
    return reciprocal(*n.m_rational);
  } else {
    return 1.0/n.getRealValue();
  }
}

Number pow(const Number &n1, const Number &n2) {
  if(n1.isRational() && n2.isInteger()) {
    return pow(*n1.m_rational, n2.getIntValue());
  } else {
    const Real r = mypow(n1.getRealValue(), n2.getRealValue());
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
  if((t1 != NUMBERTYPE_RATIONAL) && (t2 != NUMBERTYPE_RATIONAL)) {
    return rationalCmp(n1.getRationalValue(), n2.getRationalValue());
  } else {
    return sign(n1.getRealValue() - n2.getRealValue());
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
