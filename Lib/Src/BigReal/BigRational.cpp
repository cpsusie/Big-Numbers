#include "pch.h"

const ConstBigRational BigRational::_0(         BigReal::_0, BigReal::_1);  // 0
const ConstBigRational BigRational::_05(        BigReal::_1, BigReal::_2);  // 1/2
const ConstBigRational BigRational::_1(         BigReal::_1, BigReal::_1);   // 1
const ConstBigRational BigRational::_2(         BigReal::_2, BigReal::_1);   // 2
const ConstBigRational BigRational::_BRAT_QNAN( BigReal::_0, BigReal::_0);  // non-signaling NaN (quiet NaN)
const ConstBigRational BigRational::_BRAT_PINF( BigReal::_1, BigReal::_0);  // +infinity;
const ConstBigRational BigRational::_BRAT_NINF(-BigReal::_1, BigReal::_0);  // -infinity;

BigRational::BigRational(DigitPool *digitPool)
: m_numerator(  0, digitPool)
, m_denominator(1, digitPool)
{
}

BigRational::BigRational(const BigRational &r, DigitPool *digitPool)
: m_numerator(  r.m_numerator  , digitPool ? digitPool : r.getDigitPool())
, m_denominator(r.m_denominator, digitPool ? digitPool : r.getDigitPool())
{
}

BigRational::BigRational(const BigInt &numerator, const BigInt &denominator, DigitPool *digitPool)
: m_numerator(  digitPool?digitPool:numerator.getDigitPool())
, m_denominator(digitPool?digitPool:numerator.getDigitPool())
{
  init(numerator, denominator);
}
BigRational::BigRational(const BigInt &n, DigitPool *digitPool)
: m_numerator(  n          , digitPool)
, m_denominator(BigReal::_1, digitPool?digitPool:n.getDigitPool())
{
}
BigRational::BigRational(int            n, DigitPool *digitPool)
: m_numerator(  n          ,digitPool)
, m_denominator(BigReal::_1,digitPool)
{
}
BigRational::BigRational(UINT           n, DigitPool *digitPool)
: m_numerator(  n          ,digitPool)
, m_denominator(BigReal::_1,digitPool)
{
}
BigRational::BigRational(long           n, DigitPool *digitPool)
  : m_numerator(n, digitPool)
  , m_denominator(BigReal::_1, digitPool)
{
}
BigRational::BigRational(ULONG          n, DigitPool *digitPool)
  : m_numerator(n, digitPool)
  , m_denominator(BigReal::_1, digitPool)
{
}

BigRational::BigRational(INT64          n, DigitPool *digitPool)
: m_numerator(  n          ,digitPool)
, m_denominator(BigReal::_1,digitPool)
{
}

BigRational::BigRational(UINT64         n, DigitPool *digitPool)
: m_numerator(  n          ,digitPool)
, m_denominator(BigReal::_1,digitPool)
{
}

BigRational::BigRational(const _int128  &n, DigitPool *digitPool)
: m_numerator(  n          ,digitPool)
, m_denominator(BigReal::_1,digitPool)
{
}

BigRational::BigRational(const _uint128 &n, DigitPool *digitPool)
: m_numerator(  n          ,digitPool)
, m_denominator(BigReal::_1,digitPool)
{
}

BigRational::BigRational(const Rational &r, DigitPool *digitPool)
: m_numerator(  r.getNumerator()  , digitPool)
, m_denominator(r.getDenominator(), digitPool)
{
}

BigRational::BigRational(const String  &s, DigitPool *digitPool)
: m_numerator(  digitPool)
, m_denominator(digitPool)
{
  init(s);
}

BigRational::BigRational(const char    *s, DigitPool *digitPool)
: m_numerator(  digitPool)
, m_denominator(digitPool)
{
  init(s);
}

BigRational::BigRational(const wchar_t *s, DigitPool *digitPool)
: m_numerator(  digitPool)
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
  clrInitDone();
  if(!isfinite(numerator) || !isfinite(denominator)) {
    throwBigRealInvalidArgumentException(method, _T("numerator and denominator must both be finite"));
  }
  if(denominator.isZero()) {
    if(BigReal::compareAbs(numerator, BigReal::_1) <= 0) { // num = {-1,0,1} (=-inf,nan,+inf}
      m_numerator   = numerator;
      m_denominator = denominator;
      goto End;
    }
    throwInvalidArgumentException(method, _T("Denominator is zero (and numerator is not in {-1,0,1}"));
  }

  DigitPool *pool = getDigitPool();
  if(numerator.isZero()) { // zero always 0/1
    setToZero();
  } else {
    DigitPool *gcdPool = BigRealResourcePool::fetchDigitPool();
    { m_numerator   = numerator;
      m_denominator = denominator;
      if(m_denominator.isNegative()) { // Negative numbers are represented with negative numerator and positive denominator
        m_numerator.changeSign();
        m_denominator.changeSign();
      }
      const bool neg = m_numerator.isNegative();
      if(neg) m_numerator.changeSign();
      // 
      const BigInt gcd = findGCD(m_numerator,m_denominator,gcdPool);
      if(gcd > pool->_1()) {
        m_numerator   /= gcd;
        m_denominator /= gcd;
      }
      if(neg) m_numerator.changeSign();
    }
    BigRealResourcePool::releaseDigitPool(gcdPool);
  }
End:
  setInitDone();
}

BigRational &BigRational::setToZero() {
  CHECKISMUTABLE(*this);
  m_numerator.setToZero();
  m_denominator = getDigitPool()->_1();
  return *this;
}
BigRational &BigRational::setToPInf() {
  CHECKISMUTABLE(*this);
  m_denominator.setToZero();
  m_numerator = getDigitPool()->_1();
  return *this;
}
BigRational &BigRational::setToNInf() {
  setToPInf();
  m_numerator.changeSign();
  return *this;

}
BigRational &BigRational::setToNan() {
  CHECKISMUTABLE(*this);
  m_numerator.setToZero();
  m_denominator.setToZero();
  return *this;
}

BigRational &BigRational::setToNonNormalFpClass(int fpclass) {
  switch(fpclass) {
  case _FPCLASS_NZ  :
  case _FPCLASS_PZ  : return setToZero();
  case _FPCLASS_SNAN:
  case _FPCLASS_QNAN: return setToNan();
  case _FPCLASS_PINF: return setToPInf();
  case _FPCLASS_NINF: return setToNInf();
  default           : throwInvalidArgumentException(__TFUNCTION__, _T("fpclass=%04x"), fpclass);
  }
  return *this;
}

int _fpclass(const BigRational &r) {
  switch(::_fpclass(r.getDenominator())) {
  case _FPCLASS_PZ:
    switch(::_fpclass(r.getNumerator())) {
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
  return BigRational(l.m_numerator * r.m_denominator, l.m_denominator * r.m_numerator);
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

int BigRational::compare(const BigRational &r1, const BigRational &r2) { // static
  assert(r1._isfinite() && r2._isfinite());
  const int sign1 = sign(r1);
  int       c = sign1 - sign(r2);
  if(c != 0) return c;
  if(r1.getDenominator() == r2.getDenominator()) {
    return BigReal::compareAbs(r1.getNumerator(), r2.getNumerator()) * sign1;
  } else {
    BigInt p1(r1.getNumerator()); p1.clrInitDone() *= r2.getDenominator();
    BigInt p2(r2.getNumerator()); p2.clrInitDone() *= r1.getDenominator();
    return BigReal::compareAbs(p1,p2) * sign1;
  }
}

BigInt BigRational::findGCD(const BigInt &a, const BigInt &b, DigitPool *pool) { // static
  assert(a._isnormal() && b._isnormal() && a.isPositive() && b.isPositive());

  BigInt g = pool->_1();
  BigInt u(a,pool);
  BigInt v(b,pool);

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
    switch(BigReal::compareAbs(u,v)) {
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
