#include "pch.h"
#include <Math/MathFunctions.h>
#include <Math/Expression/Number.h>

DEFINECLASSNAME(Number);

#ifdef _DEBUG
bool         Number::s_debugStringEnabled = getDebuggerPresent();
#endif

Number::Number() {
  m_type     = NUMBERTYPE_UNDEFINED;
  m_rational = NULL;
  INITNUMBERDEBUGSTRING();
}

Number::Number(const Number &v) {
  m_type = v.m_type;
  switch(m_type) {
  case NUMBERTYPE_UNDEFINED:
    break;;
  case NUMBERTYPE_REAL    :
    m_real     = new Real(*v.m_real);
    break;
  case NUMBERTYPE_RATIONAL:
    m_rational = new Rational(*v.m_rational);
    break;
  default:
    throwUnknownTypeException(_T("copyConstructor"));
  }
  INITNUMBERDEBUGSTRING();
}

Number::Number(const Real &v) {
  Rational tmp;
  if(Rational::isRealRational(v, &tmp)) {
    m_type     = NUMBERTYPE_RATIONAL;
    m_rational = new Rational(tmp);
  } else if(isNan(v)) {
    m_type     = NUMBERTYPE_UNDEFINED;
    m_rational = NULL;
  } else {
    m_type     = NUMBERTYPE_REAL;
    m_real     = new Real(v);
  }
  INITNUMBERDEBUGSTRING();
}

Number::Number(const Rational &v) {
  m_type     = NUMBERTYPE_RATIONAL;
  m_rational = new Rational(v);
  INITNUMBERDEBUGSTRING();
}

Number::Number(int v) {
  m_type     = NUMBERTYPE_RATIONAL;
  m_rational = new Rational(v);
  INITNUMBERDEBUGSTRING();
}

Number::Number(unsigned int v) {
  m_type     = NUMBERTYPE_RATIONAL;
  m_rational = new Rational(v);
  INITNUMBERDEBUGSTRING();
}

Number::~Number() {
  cleanup();
}

void Number::setType(NumberType type) {
  DEFINEMETHODNAME;
  if(type == m_type) {
    return;
  }
  cleanup();
  switch(type) {
  case NUMBERTYPE_UNDEFINED:
    return;
  case NUMBERTYPE_REAL    :
    m_type     = NUMBERTYPE_REAL;
    m_real     = new Real;
    return;
  case NUMBERTYPE_RATIONAL:
    m_type     = NUMBERTYPE_RATIONAL;
    m_rational = new Rational;;
    return;
  default:
    throwUnknownTypeException(method, type);
  }
}

void Number::cleanup() {
  switch(m_type) {
  case NUMBERTYPE_UNDEFINED:
    break;;
  case NUMBERTYPE_REAL    :
    delete m_real;
    break;
  case NUMBERTYPE_RATIONAL:
    delete m_rational;
    break;
  default:
    throwUnknownTypeException(_T("cleanup"));
  }
  m_type     = NUMBERTYPE_UNDEFINED;
  m_rational = NULL;
}

void Number::throwUnknownTypeException(const TCHAR *method) const {
  throwUnknownTypeException(method, m_type);
}

void Number::throwUnknownTypeException(const TCHAR *method, NumberType type) { // static
  throwMethodException(s_className, method, _T("Unknown type:%d"), type);
}

void Number::throwTypeIsUndefinedException(const TCHAR *method) { // static
  throwMethodException(s_className, method, _T("Number is undefined"));
}

Number &Number::operator=(const Number &v) {
  if(&v == this) {
    return *this;
  }
  if(m_type != v.m_type) {
    setType(v.m_type);
  }
  switch(m_type) {
  case NUMBERTYPE_UNDEFINED:
    break;

  case NUMBERTYPE_REAL    :
    *m_real     = *v.m_real;
    break;
  
  case NUMBERTYPE_RATIONAL:
    *m_rational = *v.m_rational;
    break;

  default:
    v.throwUnknownTypeException(_T("operator="));
  }
  INITNUMBERDEBUGSTRING();
  return *this;
}

Number &Number::operator=(const Real &v) {
  Rational tmp;
  if(Rational::isRealRational(v, &tmp)) {
    if(m_type != NUMBERTYPE_RATIONAL) {
      setType(NUMBERTYPE_RATIONAL);
    }
    *m_rational = tmp;
  } else if(isNan(v)) {
    setType(NUMBERTYPE_UNDEFINED);
  } else {
    setType(NUMBERTYPE_REAL);
    *m_real     = v;
  }
  INITNUMBERDEBUGSTRING();
  return *this;
}

Number &Number::operator=(const Rational &v) {
  if(m_type != NUMBERTYPE_RATIONAL) {
    setType(NUMBERTYPE_RATIONAL);
  }
  *m_rational = v;
  INITNUMBERDEBUGSTRING();
  return *this;
}

Number &Number::operator=(int v) {
  if(m_type != NUMBERTYPE_RATIONAL) {
    setType(NUMBERTYPE_RATIONAL);
  }
  *m_rational = v;
  INITNUMBERDEBUGSTRING();
  return *this;
}

Number &Number::operator=(unsigned int v) {
  if(m_type != NUMBERTYPE_RATIONAL) {
    setType(NUMBERTYPE_RATIONAL);
  }
  *m_rational = v;
  INITNUMBERDEBUGSTRING();
  return *this;
}

void Number::forceTypeToReal() {
  DEFINEMETHODNAME;
  switch(getType()) {
  case NUMBERTYPE_UNDEFINED:
    throwTypeIsUndefinedException(method);

  case NUMBERTYPE_REAL    :
    return;
  case NUMBERTYPE_RATIONAL:
    { const Real v = getReal(*m_rational);
      setType(NUMBERTYPE_REAL);
      *m_real = v;
    }
    break;
  default:
    throwUnknownTypeException(method);
  }
  INITNUMBERDEBUGSTRING();
}

int Number::getIntValue() const {
  DEFINEMETHODNAME;
  switch(getType()) {
  case NUMBERTYPE_UNDEFINED:
    throwTypeIsUndefinedException(method);
  case NUMBERTYPE_REAL    :
    return getInt(*m_real);
  case NUMBERTYPE_RATIONAL:
    return getInt(*m_rational);
  default:
    throwUnknownTypeException(method);
    return 0;
  }
}

Real Number::getRealValue() const {
  DEFINEMETHODNAME;
  switch(getType()) {
  case NUMBERTYPE_UNDEFINED:
    throwTypeIsUndefinedException(method);
  case NUMBERTYPE_REAL    :
    return *m_real;
  case NUMBERTYPE_RATIONAL:
    return getDouble(*m_rational);
  default:
    throwUnknownTypeException(method);
    return 0;
  }
}

Rational Number::getRationalValue() const {
  DEFINEMETHODNAME;
  switch(getType()) {
  case NUMBERTYPE_UNDEFINED:
    throwTypeIsUndefinedException(method);
  case NUMBERTYPE_REAL    :
    return Rational(getDouble(*m_real));
  case NUMBERTYPE_RATIONAL:
    return *m_rational;
  default:
    throwUnknownTypeException(method);
    return 0;
  }
}

const Real *Number::getRealAddress() const {
  DEFINEMETHODNAME;
  switch(getType()) {
  case NUMBERTYPE_UNDEFINED:
    throwTypeIsUndefinedException(method);
  case NUMBERTYPE_REAL     :
    break;
  case NUMBERTYPE_RATIONAL :
    ((Number*)this)->forceTypeToReal();
    break;
  default:
    throwUnknownTypeException(method);
  }
  return m_real;
}

String Number::toString() const {
  switch(getType()) {
  case NUMBERTYPE_UNDEFINED:
    return _T("undefined");
  case NUMBERTYPE_REAL     :
    return ::toString(*m_real);
  case NUMBERTYPE_RATIONAL :
    return m_rational->toString();
  default:
    return format(_T("%s::toString:Unknown type:%d"), s_className, getType());
  }
}

bool Number::enableDebugString(bool enabled) { // static
#ifdef _DEBUG
  const bool ret = s_debugStringEnabled;
  s_debugStringEnabled = enabled;
  return ret;
#else
  return false;
#endif
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
  case NUMBERTYPE_UNDEFINED:
    Number::throwTypeIsUndefinedException(method);

  case NUMBERTYPE_REAL    :
    return -v.getRealValue();
  case NUMBERTYPE_RATIONAL:
    return -v.getRationalValue();
  default:
    Number::throwUnknownTypeException(method, v.getType());
    return 0;
  }
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
  const NumberType t1 = n1.getType();
  const NumberType t2 = n2.getType();
  if(t1 == NUMBERTYPE_UNDEFINED) {
    throwException(_T("numberCmp:n1 is undefined"));
  } else if(t2 == NUMBERTYPE_UNDEFINED) {
    throwException(_T("numberCmp:n2 is undefined"));
  }
  if((t1 != t2) || (t1 == NUMBERTYPE_REAL)) {
    return sign(n1.getRealValue() - n2.getRealValue());
  } else { // they are both NUMBERTYPE_RATIONAL
    return rationalCmp(n1.getRationalValue(), n2.getRationalValue());
  }
}

bool Number::operator==(const Number &n) const {
  if(&n == this) {
    return true;
  }
  const NumberType type = getType();
  if(type != n.getType()) {
    return false;
  }
  switch(type) {
  case NUMBERTYPE_UNDEFINED:
    throwTypeIsUndefinedException(_T("operator=="));
    return false;
  case NUMBERTYPE_REAL     :
    return getRealValue() == n.getRealValue();
  case NUMBERTYPE_RATIONAL :
    return getRationalValue() == n.getRationalValue();
  default:
    throwUnknownTypeException(_T("operator=="));
  }
  return false;
}
