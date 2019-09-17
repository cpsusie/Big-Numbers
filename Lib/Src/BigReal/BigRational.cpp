#include "pch.h"
#include <Math/BigRational.h>

BigRational::BigRational(DigitPool *digitPool) : m_numerator(digitPool), m_denominator(digitPool) {
  init(digitPool->get0(), digitPool->get1());
}

BigRational::BigRational(const BigInt &numerator, const BigInt &denominator, DigitPool *digitPool)
: m_numerator(  digitPool?digitPool:denominator.getDigitPool())
, m_denominator(digitPool?digitPool:denominator.getDigitPool()) {
  init(numerator, denominator);
}

BigRational::BigRational(const BigInt &n, DigitPool *digitPool)
: m_numerator(  n,        digitPool)
, m_denominator(BIGREAL_1, digitPool?digitPool:n.getDigitPool()) {
}

BigRational::BigRational(int            n, DigitPool *digitPool) : m_numerator(n, digitPool), m_denominator(BIGREAL_1,digitPool) {
}

BigRational::BigRational(const String  &s, DigitPool *digitPool) : m_numerator(digitPool), m_denominator(digitPool) {
  init(s);
}

BigRational::BigRational(const char    *s, DigitPool *digitPool) : m_numerator(digitPool), m_denominator(digitPool) {
  init(s);
}

BigRational::BigRational(const wchar_t *s, DigitPool *digitPool) : m_numerator(digitPool), m_denominator(digitPool) {
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
    init(BigInt(s,pool),pool->get1());
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
    m_numerator   = pool->get0();
    m_denominator = pool->get1();
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
  switch(fpclassify(r)) {
  case FP_NORMAL:
    return r.isNegative() ? _FPCLASS_NN : _FPCLASS_PN;
  case FP_ZERO:
    return r.isNegative() ? _FPCLASS_NZ : _FPCLASS_PZ;
  case FP_INFINITE:
    return r.isNegative() ? _FPCLASS_NINF : _FPCLASS_PINF;
  case FP_NAN:
    return _FPCLASS_QNAN;
  default:
    return _FPCLASS_SNAN;
  }
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

  BigInt g = pool->get1();
  BigInt u(pool);
  u = a;
  BigInt v(b);
  static const ConstBigInt two = 2;

  while(even(u) && even(v)) {
    u /= two;
    v /= two;
    g *= two;
  }

  // Now u or v (or both) are odd
  while(u.isPositive()) {
    if(even(u)) {
      u /= two;
    } else if(even(v)) {
      v /= two;
    } else if(u < v) {
      v = (v-u)/two;
    } else {
      u = (u-v)/two;
    }
  }
  return g*v;
}
