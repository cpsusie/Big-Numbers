#include "pch.h"
#include "ResourcePoolInternal.h"

BigRational::BigRational(DigitPool *digitPool)
: m_num(0, digitPool)
, m_den(1, digitPool)
{
}

BigRational::BigRational(const BigRational &r, DigitPool *digitPool)
: m_num(r.m_num, digitPool ? digitPool : r.getDigitPool())
, m_den(r.m_den, digitPool ? digitPool : r.getDigitPool())
{
}

BigRational::BigRational(const BigInt &numerator, const BigInt &denominator, DigitPool *digitPool)
: m_num(digitPool?digitPool:numerator.getDigitPool())
, m_den(digitPool?digitPool:numerator.getDigitPool())
{
  init(numerator, denominator);
}
BigRational::BigRational(const BigInt &n, DigitPool *digitPool)
: m_num(n          , digitPool)
, m_den(BigReal::_1, digitPool?digitPool:n.getDigitPool())
{
  if(!n._isfinite()) {
    setToNonNormalFpClass(_fpclass(n));
  }
}
BigRational::BigRational(int            n, DigitPool *digitPool)
: m_num(  n          ,digitPool)
, m_den(BigReal::_1,digitPool)
{
}
BigRational::BigRational(UINT           n, DigitPool *digitPool)
: m_num(  n          ,digitPool)
, m_den(BigReal::_1,digitPool)
{
}
BigRational::BigRational(long           n, DigitPool *digitPool)
  : m_num(n          , digitPool)
  , m_den(BigReal::_1, digitPool)
{
}
BigRational::BigRational(ULONG          n, DigitPool *digitPool)
  : m_num(n          , digitPool)
  , m_den(BigReal::_1, digitPool)
{
}

BigRational::BigRational(INT64          n, DigitPool *digitPool)
: m_num(n          ,digitPool)
, m_den(BigReal::_1,digitPool)
{
}

BigRational::BigRational(UINT64         n, DigitPool *digitPool)
: m_num(n          ,digitPool)
, m_den(BigReal::_1,digitPool)
{
}

BigRational::BigRational(const _int128  &n, DigitPool *digitPool)
: m_num(n          ,digitPool)
, m_den(BigReal::_1,digitPool)
{
}

BigRational::BigRational(const _uint128 &n, DigitPool *digitPool)
: m_num(n          ,digitPool)
, m_den(BigReal::_1,digitPool)
{
}

BigRational::BigRational(const Rational &r, DigitPool *digitPool)
: m_num(r.getNumerator()  , digitPool)
, m_den(r.getDenominator(), digitPool)
{
  if(!isnormal(r)) {
    setToNonNormalFpClass(_fpclass(r));
  }
}

BigRational::BigRational(const String  &s, DigitPool *digitPool)
: m_num(digitPool)
, m_den(digitPool)
{
  init(s);
}

BigRational::BigRational(const char    *s, DigitPool *digitPool)
: m_num(digitPool)
, m_den(digitPool)
{
  init(s);
}

BigRational::BigRational(const wchar_t *s, DigitPool *digitPool)
: m_num(digitPool)
, m_den(digitPool)
{
  init(s);
}

BigRational &BigRational::operator=(const BigInt &n) {
  CHECKISMUTABLE(*this);
  if(!isnormal(n)) {
    return setToNonNormalFpClass(_fpclass(n));
  }
  m_den = getDigitPool()->_1();
  m_num = n;
  return *this;
}
BigRational &BigRational::operator=(const Rational &r) {
  CHECKISMUTABLE(*this);
  if(!isnormal(r)) {
    return setToNonNormalFpClass(_fpclass(r));
  }
  m_num = r.getNumerator();
  m_den = r.getDenominator();
  return *this;
}

void BigRational::init(const String &s) {
  String tmp(s);
  const int slash = (int)tmp.find(_T('/'));
  if(slash == 0) {
    throwBigRealInvalidArgumentException(__TFUNCTION__, _T("s=%s"), s.cstr());
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
      m_num   = numerator;
      m_den = denominator;
      goto End;
    }
    throwInvalidArgumentException(method, _T("Denominator is zero (and numerator is not in {-1,0,1}"));
  }

  DigitPool *pool = getDigitPool();
  if(numerator.isZero()) { // zero always 0/1
    setToZero();
  } else {
    DigitPool *gcdPool = BigRealResourcePool::fetchDigitPool();
    try {
      m_num = numerator;
      m_den = denominator;
      if(m_den.isNegative()) { // Negative numbers are represented with negative numerator and positive denominator
        m_num.changeSign();
        m_den.changeSign();
      }
      const bool neg = m_num.isNegative();
      if(neg) m_num.flipSign();;
      const BigInt gcd = findGCD(m_num,m_den,gcdPool);
      if(gcd > pool->_1()) {
        m_num   /= gcd;
        m_den /= gcd;
      }
      if(neg) m_num.flipSign();
    } catch(...) {
      BigRealResourcePool::releaseDigitPool(gcdPool);
      throw;
    }
    BigRealResourcePool::releaseDigitPool(gcdPool);
  }
End:
  setInitDone();
}

BigRational &BigRational::setToZero() {
  CHECKISMUTABLE(*this);
  m_num.setToZero();
  m_den = getDigitPool()->_1();
  return *this;
}
BigRational &BigRational::setToPInf() {
  CHECKISMUTABLE(*this);
  m_den.setToZero();
  m_num = getDigitPool()->_1();
  return *this;
}
BigRational &BigRational::setToNInf() {
  setToPInf();
  m_num.flipSign();
  return *this;

}
BigRational &BigRational::setToNan() {
  CHECKISMUTABLE(*this);
  m_num.setToZero();
  m_den.setToZero();
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

#define NaN  BigRational(BigRational::_BRAT_QNAN, pool)
#define Pinf BigRational(BigRational::_BRAT_PINF, pool)
#define Ninf BigRational(BigRational::_BRAT_NINF, pool)

BigRational sum(const BigRational &x, const BigRational &y, DigitPool *digitPool) {
  _SELECTDIGITPOOL(x);
  if(!isfinite(x) || !isfinite(y)) return NaN;
  return BigRational(x.m_num * y.m_den + y.m_num * x.m_den, x.m_den * y.m_den, pool);
}

BigRational dif(const BigRational &x, const BigRational &y, DigitPool *digitPool) {
  _SELECTDIGITPOOL(x);
  if(!isfinite(x) || !isfinite(y)) return NaN;
  return BigRational(x.m_num * y.m_den - y.m_num * x.m_den, x.m_den * y.m_den, pool);
}

BigRational prod(const BigRational &x, const BigRational &y, DigitPool *digitPool) {
  _SELECTDIGITPOOL(x);
  if(!isfinite(x) || !isfinite(y)) return NaN;
  return BigRational(x.m_num * y.m_num, x.m_den * y.m_den, pool);
}

BigRational quot(const BigRational &x, const BigRational &y, DigitPool *digitPool) {
  _SELECTDIGITPOOL(x);
  const int lclass = _fpclass(x);
  if((lclass & (_FPCLASS_NN | _FPCLASS_PN | _FPCLASS_NZ | _FPCLASS_PZ)) == 0) {
    return NaN;
  }
  switch(fpclassify(y)) {
  case FP_NORMAL:
    return BigRational(x.m_num * y.m_den, x.m_den * y.m_num, pool);
  case FP_ZERO:
    switch(lclass) {
    case _FPCLASS_PN: return Pinf;  // +finite/0 -> +inf
    case _FPCLASS_NN: return Ninf;  // -finite/0 -> -inf
    case _FPCLASS_NZ:
    case _FPCLASS_PZ: return NaN;   // 0/0       ->  nan
    }
  }
  return NaN;
}

BigRational rem(const BigRational &x, const BigRational &y, DigitPool *digitPool) {
  _SELECTDIGITPOOL(x);
  const int lclass = _fpclass(x);
  if((lclass & (_FPCLASS_NN | _FPCLASS_PN | _FPCLASS_NZ | _FPCLASS_PZ)) == 0) {
    return NaN;
  }
  switch(fpclassify(y)) {
  case FP_NORMAL:
    { const BigInt n = (x.m_num*y.m_den) / (x.m_den*y.m_num);
      return n.isZero() ? x : (x - y * n);
    }
  case FP_ZERO:
    switch(lclass) {
    case _FPCLASS_PN: return Pinf;   // +finite%0 -> +inf
    case _FPCLASS_NN: return Ninf;   // -finite%0 -> -inf
    case _FPCLASS_NZ:
    case _FPCLASS_PZ: return NaN;    // 0%0       ->  nan
    }
  }
  return NaN;
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

BigRational BigRational::operator-() const {
  switch(_fpclass(*this)) {
  case _FPCLASS_SNAN  :
  case _FPCLASS_QNAN  : 
  case _FPCLASS_NZ    :
  case _FPCLASS_PZ    : return *this;
  case _FPCLASS_NINF  :
  case _FPCLASS_NN    :
  case _FPCLASS_ND    :
  case _FPCLASS_PD    :
  case _FPCLASS_PN    :
  case _FPCLASS_PINF  :
    return BigRational(-m_num, m_den);
  default             :
    return *this;
  }
}

BigRational fabs(const BigRational &r, DigitPool *digitPool) {
  _SELECTDIGITPOOL(r);
  if(isnan(r) || !r.getNumerator().isNegative()) return BigRational(r,pool);
  BigRational result(r, pool);
  result.m_num.setPositive();
  return result;
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

bool isInt(const BigRational &v, int      *n) {
  return v._isinteger() && isInt(v.getNumerator(), n);
}
bool isUint(const BigRational &v, UINT     *n) {
  return v._isinteger() && isUint(v.getNumerator(), n);
}
bool isInt64(const BigRational &v, INT64    *n) {
  return v._isinteger() && isInt64(v.getNumerator(), n);
}
bool isUint64(const BigRational &v, UINT64   *n) {
  return v._isinteger() && isUint64(v.getNumerator(), n);
}
bool isInt128(const BigRational &v, _int128  *n) {
  return v._isinteger() && isInt128(v.getNumerator(), n);
}
bool isUint128(const BigRational &v, _uint128 *n) {
  return v._isinteger() && isUint128(v.getNumerator(), n);
}
bool isRational(const BigRational &v, Rational *r) {
  if(!v._isnormal()) {
    if(r) *r = getNonNormalValue(_fpclass(v), Rational::_0);
    return true;
  } else if(r == NULL) {
    return isInt64(v.getNumerator()) && isInt64(v.getDenominator());
  } else { // r != NULL
    INT64 n, d;
    if(isInt64(v.getNumerator(), &n) && isInt64(v.getDenominator(), &d)) {
      *r = Rational(n,d);
      return true;
    }
    return false;
  }
}
