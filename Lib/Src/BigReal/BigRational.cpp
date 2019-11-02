#include "pch.h"

const BigRational BigRational::_0( BigReal::_0, BigReal::_1);  // 0
const BigRational BigRational::_05(BigReal::_1, BigReal::_2);  // 1/2
const BigRational BigRational::_1( BigReal::_1, BigReal::_1);   // 1
const BigRational BigRational::_2( BigReal::_2, BigReal::_1);   // 2
const BigRational BigRational::_BRAT_QNAN( BigReal::_0, BigReal::_0);  // non-signaling NaN (quiet NaN)
const BigRational BigRational::_BRAT_PINF( BigReal::_1, BigReal::_0);  // +infinity;
const BigRational BigRational::_BRAT_NINF(-BigReal::_1, BigReal::_0);  // -infinity;

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
  if (!isfinite(numerator) || !isfinite(denominator)) {
    throwBigRealInvalidArgumentException(method, _T("numerator and denominator must both be finite"));
  }
  if(denominator.isZero()) {
    if(compareAbs(numerator, BigReal::_1) <= 0) { // num = {-1,0,1} (=-inf,nan,+inf}
      m_numerator   = numerator;
      m_denominator = denominator;
      return;
    }
    throwInvalidArgumentException(method, _T("Denominator is zero (and numerator is not in {-1,0,1}"));
  }

  DigitPool *pool = getDigitPool();
  if(numerator.isZero()) { // zero always 0/1
    m_numerator   = pool->_0();
    m_denominator = pool->_1();
  } else {
    const BigInt gcd = findGCD(BigInt(fabs(numerator)),BigInt(fabs(denominator)));
    if(gcd == pool->_1()) {
      m_numerator   = numerator;
      m_denominator = denominator;
    } else {
      m_numerator   = numerator / gcd;
      m_denominator = denominator / gcd;
    }
    if(denominator.isNegative()) { // Negative numbers are represented with negative numerator and positive denominator
      m_numerator.changeSign();
      m_denominator.changeSign();
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

int sign(const BigRational &r) {
  return ::sign(r.getNumerator());
}

int bigRationalCmp(const BigRational &r1, const BigRational &r2) {
  assert(isfinite(r1) && isfinite(r2));
  const int sign1 = sign(r1);
  int       c = sign1 - sign(r2);
  if(c != 0) return c;
  if(r1.getDenominator() == r2.getDenominator()) {
    return compareAbs(r1.getNumerator(), r2.getNumerator()) * sign1;
  } else {
    BigInt p1(r1.getNumerator()); p1 *= r2.getDenominator();
    BigInt p2(r2.getNumerator()); p2 *= r1.getDenominator();
    return compareAbs(p1,p2) * sign1;
  }
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
  u = a;
  BigInt v(b);

  while(isEven(u) && isEven(v)) {
    u.divide2();
    v.divide2();
    g.multiply2();
  }

  // Now u or v (or both) are odd
  while(isEven(u)) u.divide2();
  while(isEven(v)) v.divide2();
  // both u and v are odd
  for(;;) {
    switch(compareAbs(u,v)) {
    case  1: // u > v
      u -= v;
      do { // u even and > 0
        u.divide2();
      } while(isEven(u));
      continue;
    case -1: // u < v
      v -= u;
      do { // v even and > 0
        v.divide2();
      } while(isEven(v));
      continue;
    default:
      return g * v;
    }
  }
}
