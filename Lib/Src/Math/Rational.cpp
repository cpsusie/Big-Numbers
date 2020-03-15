#include "pch.h"
#include <limits.h>
#include <Math/Int128.h>
#include <Math/Double80.h>
#include <Math/Rational.h>
#include <Math/PrimeFactors.h>

const Rational Rational::_0(0,1);                   // = 0
const Rational Rational::_05(1, 2);                 // = 1/2
const Rational Rational::_1(1,1);                   // = 1
const Rational Rational::_RAT_MIN(  1,_I64_MAX);    // Min positive value (=1/_I64_MAX)
const Rational Rational::_RAT_MAX(_I16_MAX    );    // Max value          (=_I64_MAX)
const Rational Rational::_RAT_NAN(  0,0       );    // nan (undefined)    ( 0/0)
const Rational Rational::_RAT_PINF( 1,0       );    // +infinity;         ( 1/0)
const Rational Rational::_RAT_NINF(-1,0       );    // -infinity;         (-1/0)

#define SAFESUM( a,b) Rational::safeSum( __TFUNCTION__,__LINE__,a,b)
#define SAFEDIF( a,b) Rational::safeDif( __TFUNCTION__,__LINE__,a,b)
#define SAFEPROD(a,b) Rational::safeProd(__TFUNCTION__,__LINE__,a,b)

Double80 getDouble80(const Rational &r) {
  return Double80(r.getNumerator()) / r.getDenominator();
}

int fpclassify(const Rational &r) {
  switch(_fpclass(r)) {
  case _FPCLASS_PN  :
  case _FPCLASS_NN  : return FP_NORMAL;
  case _FPCLASS_PD  :
  case _FPCLASS_ND  : return FP_SUBNORMAL;
  case _FPCLASS_PZ  :
  case _FPCLASS_NZ  : return FP_ZERO;
  case _FPCLASS_NINF:
  case _FPCLASS_PINF: return FP_INFINITE;
  default           : return FP_NAN;
  }
}

int _fpclass(const Rational &r) {
  switch(sign(r.getDenominator())) {
  case 0:
    switch(sign(r.getNumerator())) {
    case  1: return _FPCLASS_PINF;
    case -1: return _FPCLASS_NINF;
    case  0: return _FPCLASS_QNAN;
    }
    break;
  case 1:
    switch(sign(r.getNumerator())) {
    case  1: return _FPCLASS_PN;
    case -1: return _FPCLASS_NN;
    case  0: return _FPCLASS_PZ;
    }
    break;
  }
  return _FPCLASS_SNAN; // denominator MUST be >= 0
}

#define CHECKISNORMAL(f)                     \
if(!isnormal(f)) {                           \
  if(isfinite(f))         *this = _0;        \
  else if(isPInfinity(f)) *this = _RAT_PINF; \
  else if(isNInfinity(f)) *this = _RAT_NINF; \
  else                    *this = _RAT_NAN;  \
  return;                                    \
}

Rational::Rational(float f, UINT maxND) {
  static const TCHAR *invalidFloatMsg = _T("Value %e cannot be contained in a Rational with maxND=%lu");
  CHECKISNORMAL(f)
  bool   positive;
  float  fa;
  if(f > 0) {
    positive = true;
    fa = f;
  } else {
    positive = false;
    fa = -f;
  }

  INT64 p0 = 1, q0 = 0;
  INT64 p1 = (INT64)floor(fa);
  INT64 q1 = 1;
  INT64 p2, q2;

  float frac = fa - p1;
  while(frac) {
    frac = 1.0f / frac;
    const float next = floor(frac);
    p2 = (INT64)(next * p1 + p0);
    q2 = (INT64)(next * q1 + q0);

    // Limit the numerator and denominator to be maxND or less
    if((p2 > maxND) || (q2 > maxND)) {
      break;
    }

    // remember the last two fractions
    p0 = p1; q0 = q1;
    p1 = p2; q1 = q2;

    frac -= next;
  }

  if((fa > maxND) || (fa < 1.0f / maxND)) { // hard upper and lower bounds for ratio
    throwInvalidArgumentException(__TFUNCTION__, invalidFloatMsg, f, maxND);
  }

  assert(((float)p1 / q1 <= maxND) && ((float)q1 / p1 <= maxND));
  assert(findGCD(p1,q1) == 1);

  m_num   = positive ? p1 : -p1;
  m_den = q1;
}
// This algorithm is borrowed from gimpzoommodel.c in LIBGIMP:
// http://svn.gnome.org/viewcvs/gimp/trunk/libgimpwidgets/gimpzoommodel.c
//
// See also http://www.virtualdub.org/blog/pivot/entry.php?id=81
// for a discussion of calculating continued fractions by convergeants.
Rational::Rational(double d, UINT maxND) {
  static const TCHAR *invalidDoubleMsg = _T("Value %le cannot be contained in a Rational with maxND=%lu");

  CHECKISNORMAL(d)
  bool   positive;
  double da;
  if(d > 0) {
    positive = true;
    da = d;
  } else {
    positive = false;
    da = -d;
  }

  INT64 p0 = 1, q0 = 0;
  INT64 p1 = (INT64)floor(da);
  INT64 q1 = 1;
  INT64 p2, q2;

  double frac = da - p1;
  while(frac) {
    frac = 1.0 / frac;
    const double next = floor(frac);
    p2 = (INT64)(next * p1 + p0);
    q2 = (INT64)(next * q1 + q0);

    // Limit the numerator and denominator to be maxND or less
    if((p2 > maxND) || (q2 > maxND)) {
      break;
    }

    // remember the last two fractions
    p0 = p1; q0 = q1;
    p1 = p2; q1 = q2;

    frac -= next;
  }

  if((da > maxND) || (da < 1.0 / maxND)) { // hard upper and lower bounds for ratio
    throwInvalidArgumentException(__TFUNCTION__, invalidDoubleMsg, d, maxND);
  }

  assert(((double)p1 / q1 <= maxND) && ((double)q1 / p1 <= maxND));
  assert(findGCD(p1,q1) == 1);

  m_num = positive ? p1 : -p1;
  m_den = q1;
}

Rational::Rational(const Double80 &d80, UINT64 maxND) {
  static const TCHAR *invalidDoubleMsg = _T("Value %s cannot be contained in a Rational with maxND=%I64u");

  CHECKISNORMAL(d80)
  bool     positive;
  Double80 da;
  if(d80.isPositive()) {
    positive = true;
    da = d80;
  } else {
    positive = false;
    da = -d80;
  }

  UINT64 p0 = 1, q0 = 0;
  UINT64 p1 = getInt64(floor(da));
  UINT64 q1 = 1;
  UINT64 p2, q2;

  Double80 frac = da - p1;
  while(!frac.isZero()) {
    frac = Double80::_1 / frac;
    const Double80 next = floor(frac);
    p2 = getInt64(next * p1 + p0);
    q2 = getInt64(next * q1 + q0);

    // Limit the numerator and denominator to be maxND or less
    if((p2 > maxND) || (q2 > maxND)) {
      break;
    }

    // remember the last two fractions
    p0 = p1; q0 = q1;
    p1 = p2; q1 = q2;

    frac -= next;
  }

  if((da > maxND) || (da < Double80::_1 / maxND)) { // hard upper and lower bounds for ratio
    throwInvalidArgumentException(__TFUNCTION__, invalidDoubleMsg, toString(d80).cstr(), maxND);
  }

  assert(((Double80)p1 / q1 <= maxND) && ((Double80)q1 / p1 <= maxND));
  assert(findGCD(p1,q1) == 1);

  m_num = positive ? p1 : -(INT64)p1;
  m_den = q1;
}

void Rational::init(INT64 numerator, INT64 denominator) {
  DEFINEMETHODNAME;
  if(denominator == 0) {
    switch(numerator) {
    case -1: // -Infinity
    case  0: // NaN
    case  1: // +Infinity
      m_den = denominator;
      m_num = numerator;
      return;
    default:
      throwInvalidArgumentException(method, _T("Denominator is zero"));
    }
  } else if((numerator < -_I64_MAX) || (numerator   > _I64_MAX)) {
    throwInvalidArgumentException(method, _T("Numerator(=%I64d) out of range [%I64d..%I64d]"), numerator, -_I64_MAX, _I64_MAX);
  } else if((denominator < -_I64_MAX) || (denominator > _I64_MAX)) {
    throwInvalidArgumentException(method, _T("Denominator(=%I64d) out of range [%I64d..%I64d]"), denominator, -_I64_MAX, _I64_MAX);
  }

  if(numerator == 0) {    // zero always 0/1
    m_num = 0;
    m_den = 1;
  } else {
    if(denominator < 0) { // Negative numbers are represented with negative numerator and positive denominator
      numerator   = -numerator;
      denominator = -denominator;
    }
    const INT64 gcd = findGCD(UINT64(abs(numerator)),denominator);
    if(gcd == 1) {
      m_num = numerator;
      m_den = denominator;
    } else {
      m_num = numerator   / gcd;
      m_den = denominator / gcd;
    }
  }
}

Rational operator+(const Rational &l, const Rational &r) {
  if(!isfinite(l) || !isfinite(r)) return RAT_NAN;
  try {
    if(l.m_den == r.m_den) {                                     // l.d == r.d. just add l.n and r.n
      return Rational(SAFESUM(l.m_num,r.m_num), l.m_den);
    } else if((l.m_den > r.m_den) && (l.m_den % r.m_den == 0)) { // l.d = n * r.d. extend r with n and use l.d as denominator
      const INT64 n = l.m_den / r.m_den;
      return Rational(SAFESUM(l.m_num,SAFEPROD(n,r.m_num)), l.m_den);
    } else if((l.m_den < r.m_den) && (r.m_den % l.m_den == 0)) { // r.d = n * l.d. extend l with n and use r.d as denominator
      const INT64 n = r.m_den / l.m_den;
      return Rational(SAFESUM(SAFEPROD(n,l.m_num),r.m_num), r.m_den);
    } else {                                                     // Extend both and use l.d * r.d as denominator
      return Rational(SAFESUM(SAFEPROD(l.m_num,r.m_den),SAFEPROD(r.m_num,l.m_den)), SAFEPROD(l.m_den,r.m_den));
    }
  } catch(Exception e) {
    return Rational::sum128(l, r);
  }
}

Rational operator-(const Rational &l, const Rational &r) {
  if(!isfinite(l) || !isfinite(r)) return RAT_NAN;
  try {
    if(l.m_den == r.m_den) {                                     // l.d == r.d. just subtract r.n from l.n
      return Rational(SAFEDIF(l.m_num,r.m_num), l.m_den);
    } else if((l.m_den > r.m_den) && (l.m_den % r.m_den == 0)) { // l.d = n * r.d. extend r with n and use l.d as denominator
      const INT64 n = l.m_den / r.m_den;
      return Rational(SAFEDIF(l.m_num,SAFEPROD(n,r.m_num)), l.m_den);
    } else if((l.m_den < r.m_den) && (r.m_den % l.m_den == 0)) { // r.d = n * l.d. extend l with n and use r.d as denominator
      const INT64 n = r.m_den / l.m_den;
      return Rational(SAFEDIF(SAFEPROD(n,l.m_num),r.m_num), r.m_den);
    } else {                                                     // Extend both and use l.d * r.d as denominator
      return Rational(SAFEDIF(SAFEPROD(l.m_num,r.m_den),SAFEPROD(r.m_num,l.m_den)), SAFEPROD(l.m_den,r.m_den));
    }
  } catch(Exception e) {
    return Rational::dif128(l, r);
  }
}

Rational operator-(const Rational &r) {
  switch(_fpclass(r)) {
  case _FPCLASS_SNAN  :
  case _FPCLASS_QNAN  : return r;
  case _FPCLASS_NZ    :
  case _FPCLASS_PZ    : return Rational::_0;
  case _FPCLASS_NINF  :
  case _FPCLASS_NN    :
  case _FPCLASS_ND    :
  case _FPCLASS_PD    :
  case _FPCLASS_PN    :
  case _FPCLASS_PINF  :
    return Rational(-r.m_num, r.m_den);
  default             :
    return r;
  }
}

Rational operator*(const Rational &l, const Rational &r) {
  if(!isfinite(l) || !isfinite(r)) return RAT_NAN;
  try {
    return Rational(SAFEPROD(l.m_num,r.m_num), SAFEPROD(l.m_den,r.m_den));
  } catch(Exception e) {
    return Rational::prod128(l, r);
  }
}

Rational operator/(const Rational &l, const Rational &r) {
  const int lclass = _fpclass(l);
  if((lclass & (_FPCLASS_NN | _FPCLASS_PN | _FPCLASS_NZ | _FPCLASS_PZ)) == 0) {
    return Rational::_RAT_NAN;
  }
  switch(fpclassify(r)) {
  case FP_NORMAL:
    try {
      return Rational(SAFEPROD(l.m_num, r.m_den), SAFEPROD(l.m_den, r.m_num));
    } catch (Exception) {
      return Rational::prod128(l, reciprocal(r));
    }
  case FP_ZERO:
    switch(lclass) {
    case _FPCLASS_PN: return Rational::_RAT_PINF;   // +finite/0 -> +inf
    case _FPCLASS_NN: return Rational::_RAT_NINF;   // -finite/0 -> -inf
    case _FPCLASS_NZ:
    case _FPCLASS_PZ: return Rational::_RAT_NAN;    // 0/0       ->  nan
    }
  }
  return Rational::_RAT_NAN;
}

Rational operator%(const Rational &l, const Rational &r) {
  const int lclass = _fpclass(l);
  if((lclass & (_FPCLASS_NN | _FPCLASS_PN | _FPCLASS_NZ | _FPCLASS_PZ)) == 0) {
    return Rational::_RAT_NAN;
  }
  switch(fpclassify(r)) {
  case FP_NORMAL:
    try {
      const INT64 n = SAFEPROD(l.getNumerator(), r.getDenominator()) / SAFEPROD(l.getDenominator(), r.getNumerator());
      return (n == 0) ? l : (l - r * n);
    } catch(Exception) {
      return Rational::mod128(l, r);
    }
  case FP_ZERO:
    switch (lclass) {
    case _FPCLASS_PN: return Rational::_RAT_PINF;   // +finite%0 -> +inf
    case _FPCLASS_NN: return Rational::_RAT_NINF;   // -finite%0 -> -inf
    case _FPCLASS_NZ:
    case _FPCLASS_PZ: return Rational::_RAT_NAN;    // 0%0       ->  nan
    }
  }
  return Rational::_RAT_NAN;
}

Rational fabs(const Rational &r) {
  if(isnan(r)) return RAT_NAN;
  Rational result;
  result.m_num = abs(r.m_num);
  result.m_den = r.m_den;
  return result;
}

INT64 Rational::pow(INT64 base, UINT e) {
  INT64 p = 1;
  while(e > 0) {
    if(::isEven(e)) {
      base = SAFEPROD(base,base);
      e /= 2;
    } else {
      p = SAFEPROD(p,base);
      e--;
    }
  }
  return p;
}

// return NaN if !isfinite(base)
// if(e==0) return NaN for base == 0, else 1
// if(e< 0) return +inf for base == 0
Rational pow(const Rational &base, int e) {
  if(!isfinite(base)) return RAT_NAN;
  switch(sign(e)) {
  case 0: // e == 0
    return base.isZero()
         ? RAT_NAN
         : Rational::_1;
  case -1: // e < 0
    return base.isZero()
         ? RAT_PINF
         : Rational(Rational::pow(base.m_den,-e), Rational::pow(base.m_num,-e));
  default: // e > 0
    return Rational(Rational::pow(base.m_num,e), Rational::pow(base.m_den,e));
  }
}

Rational reciprocal(const Rational &r) {
  switch(fpclassify(r)) {
  case FP_NORMAL   : return Rational(r.m_den, r.m_num);
  case FP_ZERO     : return Rational::_RAT_PINF;
  case FP_INFINITE : return Rational::_RAT_NAN;
  case FP_NAN      : return r;
  default          : throwException(_T("%s:Unknown value return from fpclassify:%d"), __TFUNCTION__, fpclassify(r));
                     return Rational::_0;
  }
}

int rationalCmp(const Rational &r1, const Rational &r2) {
  assert(isfinite(r1) && isfinite(r2));
  const int sign1 = sign(r1);
  int       c     = sign1 - sign(r2);
  if(c != 0) return c;
  _uint128 p1(abs(r1.m_num)); p1 *= r2.m_den;
  _uint128 p2(abs(r2.m_num)); p2 *= r1.m_den;
  return uint128HashCmp(p1, p2) * sign1;
}

#define CHECKVALIDRANGE(n128)                                                       \
if(!isInt64(n128)) {                                                                \
  throwInvalidArgumentException(__TFUNCTION__                                       \
                               ,_T("%s,line %d: result overflow. a=%I64d, b=%I64d") \
                               ,method, line,a, b);                                 \
}

INT64 Rational::safeSum(const TCHAR *method, int line, const INT64 &a, const INT64 &b) { // static
  if(a == 0) return b; else if(b == 0) return a;
  _int128 result(a);
  result += b;
  CHECKVALIDRANGE(result)
  return result;
}

INT64 Rational::safeDif(const TCHAR *method, int line, const INT64 &a, const INT64 &b) { // static
  if(a == 0) return -b; else if(b == 0) return a;
  _int128 result(a);
  result -= b;
  CHECKVALIDRANGE(result)
  return result;
}

INT64 Rational::safeProd(const TCHAR *method, int line, const INT64 &a, const INT64 &b) { // static
  if((a|b) == 0) return 0;
  _int128 result(a);
  result *= b;
  CHECKVALIDRANGE(result)
  return result;
}

template<typename T, typename signedT> T findGCDTemplate(T a, T b) {
  if((a|b) == 0) {
    throwInvalidArgumentException(__TFUNCTION__, _T("a=b=0"));
  }
  int shift = 0;
  while(((a|b)&1)==0) { // while a, b both even
    a >>= 1;
    b >>= 1;
    shift++;
  }

  // Now a or b (or both) are odd
  while((a&1)==0) a >>= 1;
  while((b&1)==0) b >>= 1;
  for(;;) { // a is odd and b is odd
    switch(sign((signedT&)b-(signedT&)a)) {
    case  1: // a < b
      b -= a;
      do { // b even and > 0
        b >>= 1;
      } while((b & 1) == 0);
      continue;
    case -1: // a > b
      a -= b;
      do { // a even and > 0
        a >>= 1;
      } while((a & 1) == 0);
      continue;
    default:
      return b<<shift;
    }
  }
}

UINT64 Rational::findGCD(UINT64 a, UINT64 b) { // static
  return findGCDTemplate<UINT64, INT64>(a, b);
}

inline _uint128 findGCD128(const _uint128 &a, const _uint128 &b) {
  return findGCDTemplate<_uint128, _int128>(a, b);
}

static Rational makeRationalFromI128(const TCHAR *method, int line, const _int128 &n, const _uint128 &d) {
  if(n == 0) {
    return Rational::_0;
  }
  _uint128 un, ud=d;
  bool neg = false;
  if(n < 0) {
    neg = true;
    un = -n;
  } else {
    un = n;
  }
  _int128 gcd = findGCD128(un, ud);
  if(gcd > 1) {
    un /= gcd;
    ud /= gcd;
  }
  _int128 nn = un;
  if(neg) nn = -nn;
  if(!isInt64(nn) || !isInt64(ud)) {
    throwInvalidArgumentException(__TFUNCTION__
                                 ,_T("%s,line %d: result overflow. n=%s, d=%s")
                                 ,method, line
                                 ,toString(n).cstr(), toString(d).cstr());
  }
  return Rational((INT64)nn, (INT64)ud);
}

Rational Rational::sum128(const Rational &l, const Rational &r) { // static
  const _int128  n1 = (_int128 )l.getNumerator()   * r.getDenominator();
  const _int128  n2 = (_int128 )r.getNumerator()   * l.getDenominator();
  const _uint128 d  = (_uint128)l.getDenominator() * r.getDenominator();
  return makeRationalFromI128(__TFUNCTION__,__LINE__,n1 + n2, d);
}
Rational Rational::dif128(const Rational &l, const Rational &r) { // static
  const _int128  n1 = (_int128 )l.getNumerator()   * r.getDenominator();
  const _int128  n2 = (_int128 )r.getNumerator()   * l.getDenominator();
  const _uint128 d  = (_uint128)l.getDenominator() * r.getDenominator();
  return makeRationalFromI128(__TFUNCTION__, __LINE__, n1 - n2, d);
}

Rational Rational::prod128(const Rational &l, const Rational &r) { // static
  const _int128  n = (_int128 )l.getNumerator()    * r.getNumerator();
  const _uint128 d = (_uint128)l.getDenominator()  * r.getDenominator();
  return makeRationalFromI128(__TFUNCTION__, __LINE__, n, d);
}
Rational Rational::mod128(const Rational &l, const Rational &r) {  // static
  const _int128 q = ((_int128)l.getNumerator()*r.getDenominator()) / ((_int128)l.getDenominator()*r.getNumerator());
  if(q == 0) return l;
  return dif128(l, makeRationalFromI128(__TFUNCTION__, __LINE__, q*r.getNumerator(), r.getDenominator()));
}

#define CHECKISFINITE1(f)                   \
if(!isfinite(f)) {                          \
  if(r) {                                   \
    if(isPInfinity(f))      *r = _RAT_PINF; \
    else if(isNInfinity(f)) *r = _RAT_NINF; \
    else                    *r = _RAT_NAN;  \
  }                                         \
  return true;                              \
}

#define RETURNTRUE(v) { if(r!=NULL) *r = (v); return true; }
bool Rational::isRational(float x, Rational *r) { // static
  CHECKISFINITE1(x);
  if(x == floor(x)) {
    if(fabs(x) > LLONG_MAX) {
      return false;
    }
    RETURNTRUE(getInt64(x));
  }
  try {
    const Rational tmp(x);
    if(tmp.getDenominator() <= 400) { // This is not good enough, but how should it be done ?
      RETURNTRUE(tmp);
    } else {
      return false;
    }
  } catch(Exception) {
    return false;
  }
}

bool Rational::isRational(double x, Rational *r) { // static
  CHECKISFINITE1(x);
  if(x == floor(x)) {
    if(fabs(x) > LLONG_MAX) {
      return false;
    }
    RETURNTRUE(getInt64(x));
  }
  try {
    const Rational tmp(x);
    if(tmp.getDenominator() <= 400) { // This is not good enough, but how should it be done ?
      RETURNTRUE(tmp);
    } else {
      return false;
    }
  } catch(Exception) {
    return false;
  }
}

bool Rational::isRational(const Double80 &x, Rational *r) { // static
  CHECKISFINITE1(x);
  if(x == floor(x)) {
    if(fabs(x) > LLONG_MAX) {
      return false;
    }
    RETURNTRUE(getInt64(x));
  }
  try {
    const Rational tmp(x);
    if(tmp.getDenominator() <= 400) { // This is not good enough, but how should it be done ?
      RETURNTRUE(tmp);
    } else {
      return false;
    }
  } catch(Exception) {
    return false;
  }
}

bool Rational::isRationalPow(const Rational &base, const Rational &e, Rational *r) {
  try {
    if(base.isZero()) {
      if(!e.isPositive()) {
        if(r) RETURNTRUE(_RAT_NAN);
        return false;
      } else {
        RETURNTRUE(0);
      }
    }
    if(isInt(e)) {
      RETURNTRUE(::pow(base, getInt(e)));
    } else {
      const __int64 &bn = base.getNumerator();
      const __int64 &bd = base.getDenominator();
      const __int64 &en = e.getNumerator();
      const __int64 &ed = e.getDenominator();

      if(!isInt(ed) || !isInt(en) || ((bn < 0) && ::isEven(ed))) {
        return false;
      }
      PrimeFactorArray bnFactors(bn);
      PrimeFactorArray bdFactors(bd);

      const UINT ed32 = (UINT)ed;
      if(bnFactors.hasFactorsWithNonDividableMultiplicity(ed32)
      || bdFactors.hasFactorsWithNonDividableMultiplicity(ed32)) {
        return false;
      }

      if(r != NULL) {
        for(size_t i = 0; i < bnFactors.size(); i++) {
          bnFactors[i].m_multiplicity /= ed32;
        }
        for(size_t i = 0; i < bdFactors.size(); i++) {
          bdFactors[i].m_multiplicity /= ed32;
        }

        const UINT enU32 = abs((int)en);
        if(enU32 != 1) {
          for(size_t i = 0; i < bnFactors.size(); i++) {
            bnFactors[i].m_multiplicity *= enU32;
          }
          for(size_t i = 0; i < bdFactors.size(); i++) {
            bdFactors[i].m_multiplicity *= enU32;
          }
        }
        if(::isEven(en)) bnFactors.setPositive();
        const __int64 Rn = bnFactors.getProduct();
        const __int64 Rd = bdFactors.getProduct();
        RETURNTRUE( (en < 0) ? Rational(Rd,Rn) : Rational(Rn,Rd));
      }
      return true;
    }
  } catch(Exception) {
    return false;
  }
}
