#include "pch.h"

BigRational::BigRational(DigitPool *digitPool)
: m_numerator(digitPool)
, m_denominator(digitPool)
{
  init(digitPool->_0(), digitPool->_1());
}

BigRational::BigRational(const BigInt &numerator, const BigInt &denominator, DigitPool *digitPool)
: m_numerator(  digitPool?digitPool:denominator.getDigitPool())
, m_denominator(digitPool?digitPool:denominator.getDigitPool())
{
  init(numerator, denominator);
}

BigRational::BigRational(const BigInt &n, DigitPool *digitPool)
: m_numerator(  n          , digitPool)
, m_denominator(BigReal::_1, digitPool?digitPool:n.getDigitPool())
{
}

BigRational::BigRational(int            n, DigitPool *digitPool)
: m_numerator(n, digitPool)
, m_denominator(BigReal::_1,digitPool)
{
}

BigRational::BigRational(const String  &s, DigitPool *digitPool)
: m_numerator(digitPool)
, m_denominator(digitPool)
{
  init(s);
}

BigRational::BigRational(const char    *s, DigitPool *digitPool)
: m_numerator(digitPool)
, m_denominator(digitPool)
{
  init(s);
}

BigRational::BigRational(const wchar_t *s, DigitPool *digitPool)
 : m_numerator(digitPool)
 , m_denominator(digitPool)
{
  init(s);
}

void BigRational::init(const String &s) {
  DEFINEMETHODNAME;
  String tmp(s);
  const int slash = (int)tmp.find(_T('/'));
  if(slash == 0) {
    throwBigRealInvalidArgumentException(method, _T("s=%s"), s.cstr());
  }
  DigitPool *pool = getDigitPool();
  if(slash < 0) {
    init(BigInt(s,pool),pool->_1());
  } else {
    BigInt n(substr(tmp, 0, slash), pool);
    BigInt d(substr(tmp,slash+1, tmp.length()), pool);
    init(n, d);
  }
}

void BigRational::init(const BigInt &numerator, const BigInt &denominator) {
  DEFINEMETHODNAME;
  if(denominator.isZero()) {
    throwInvalidArgumentException(method, _T("Denominator is zero"));
  }

  DigitPool *pool = getDigitPool();
  if(numerator.isZero()) { // zero always 0/1
    m_numerator   = pool->_0();
    m_denominator = pool->_1();
  } else {
    const BigInt gcd = findGCD(BigInt(fabs(numerator)),BigInt(fabs(denominator)));
    m_numerator   = numerator / gcd;
    m_denominator = denominator / gcd;
    if(denominator.isNegative()) { // Negative numbers are represented with negative numerator and positive denominator
      m_numerator   = -m_numerator;
      m_denominator = -m_denominator;
    }
  }
}

int _fpclass(const BigRational &r) {
  switch(::_fpclass(r.getDenominator())) {
  case _FPCLASS_PZ:
    switch (::_fpclass(r.getNumerator())) {
    case _FPCLASS_PZ: return _FPCLASS_QNAN;
    case _FPCLASS_PN: return _FPCLASS_PINF;
    case _FPCLASS_NN: return _FPCLASS_NINF;
    }
    break;
  case _FPCLASS_PN:
    switch(::_fpclass(r.getNumerator())) {
    case _FPCLASS_PZ: return _FPCLASS_PZ;
    case _FPCLASS_PN: return _FPCLASS_PN;
    case _FPCLASS_NN: return _FPCLASS_NN;
    }
  }
  return _FPCLASS_QNAN;
}

BigRational operator+(const BigRational &l, const BigRational &r) {
  return BigRational(l.m_numerator * r.m_denominator + r.m_numerator * l.m_denominator, l.m_denominator * r.m_denominator);
}

BigRational operator-(const BigRational &l, const BigRational &r) {
  return BigRational(l.m_numerator * r.m_denominator - r.m_numerator * l.m_denominator, l.m_denominator * r.m_denominator);
}

BigRational operator-(const BigRational &r) {
  return BigRational(-r.m_numerator, r.m_denominator);
}

BigRational operator*(const BigRational &l, const BigRational &r) {
  return BigRational(l.m_numerator * r.m_numerator, l.m_denominator * r.m_denominator);
}

BigRational operator/(const BigRational &l, const BigRational &r) {
  return BigRational(l.m_numerator * r.m_denominator ,l.m_denominator * r.m_numerator);
}

BigRational &BigRational::operator+=(const BigRational &r) {
  return *this = BigRational(m_numerator * r.m_denominator + r.m_numerator * m_denominator, m_denominator * r.m_denominator);
}

BigRational &BigRational::operator-=(const BigRational &r) {
  return *this = BigRational(m_numerator * r.m_denominator - r.m_numerator * m_denominator, m_denominator * r.m_denominator);
}

BigRational &BigRational::operator*=(const BigRational &r) {
  return *this = BigRational(m_numerator * r.m_numerator,m_denominator * r.m_denominator);
}

BigRational &BigRational::operator/=(const BigRational &r) {
  return *this = BigRational(m_numerator * r.m_denominator ,m_denominator * r.m_numerator);
}

const BigInt &BigRational::getNumerator() const {
  return m_numerator;
}

const BigInt &BigRational::getDenominator() const {
  return m_denominator;
}

BigInt BigRational::findGCD(const BigInt &a, const BigInt &b) { // static
  DigitPool *pool = b.getDigitPool();

  BigInt g = pool->_1();
  BigInt u(pool);
  const BigInt &_2 = BigReal::_2;
  u = a;
  BigInt v(b);

  while(even(u) && even(v)) {
    u /= _2;
    v /= _2;
    g *= _2;
  }

  // Now u or v (or both) are odd
  while(u.isPositive()) {
    if(even(u)) {
      u /= _2;
    } else if(even(v)) {
      v /= _2;
    } else if(u < v) {
      v = (v-u)/_2;
    } else {
      u = (u-v)/_2;
    }
  }
  return g*v;
}
