#include "pch.h"
#include <limits.h>
#include <Math/Int128.h>
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

#define SAFEPROD(a,b) Rational::safeProd(a,b,__LINE__)

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

  m_numerator   = positive ? p1 : -p1;
  m_denominator = q1;
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

  m_numerator   = positive ? p1 : -p1;
  m_denominator = q1;
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

  m_numerator   = positive ? p1 : -(INT64)p1;
  m_denominator = q1;
}

void Rational::init(const INT64 &numerator, const INT64 &denominator) {
  DEFINEMETHODNAME;
  if(denominator == 0) {
    switch(numerator) {
    case -1: // -Infinity
    case  0: // NaN
    case  1: // +Infinity
      m_denominator = denominator;
      m_numerator   = numerator;
      return;
    default:
      throwInvalidArgumentException(method, _T("Denominator is zero"));
    }
  } else if((numerator < -_I64_MAX) || (numerator   > _I64_MAX)) {
    throwInvalidArgumentException(method, _T("Numerator(=%I64d) out of range [%I64d..%I64d]"), numerator, -_I64_MAX, _I64_MAX);
  } else if((denominator < -_I64_MAX) || (denominator > _I64_MAX)) {
    throwInvalidArgumentException(method, _T("Denominator(=%I64d) out of range [%I64d..%I64d]"), denominator, -_I64_MAX, _I64_MAX);
  }

  if(numerator == 0) { // zero always 0/1
    m_numerator   = 0;
    m_denominator = 1;
  } else {
    const INT64 gcd = findGCD(UINT64(abs(numerator)),UINT64(abs(denominator)));
    m_numerator   = numerator   / gcd;
    m_denominator = denominator / gcd;
    if(denominator < 0) { // Negative numbers are represented with negative numerator and positive denominator
      m_numerator   = -m_numerator;
      m_denominator = -m_denominator;
    }
  }
}

Rational operator+(const Rational &l, const Rational &r) {
  if(!isfinite(l) || !isfinite(r)) return RAT_NAN;
  if(l.m_denominator == r.m_denominator) {                                                     // l.d == r.d. just add l.n and r.n
    return Rational(l.m_numerator + r.m_numerator, l.m_denominator);
  } else if((l.m_denominator > r.m_denominator) && (l.m_denominator % r.m_denominator == 0)) { // l.d = n * r.d. extend r with n and use l.d as denominator
    const INT64 n = l.m_denominator / r.m_denominator;
    return Rational(l.m_numerator + SAFEPROD(n,r.m_numerator), l.m_denominator);
  } else if((l.m_denominator < r.m_denominator) && (r.m_denominator % l.m_denominator == 0)) { // r.d = n * l.d. extend l with n and use r.d as denominator
    const INT64 n = r.m_denominator / l.m_denominator;
    return Rational(SAFEPROD(n,l.m_numerator) + r.m_numerator, r.m_denominator);
  } else {                                                                                     // Extend both and use l.d * r.d as denominator
    return Rational(SAFEPROD(l.m_numerator,r.m_denominator) + SAFEPROD(r.m_numerator,l.m_denominator), SAFEPROD(l.m_denominator,r.m_denominator));
  }
}

Rational operator-(const Rational &l, const Rational &r) {
  if(!isfinite(l) || !isfinite(r)) return RAT_NAN;
  if(l.m_denominator == r.m_denominator) {                                                     // l.d == r.d. just subtract r.n from l.n
    return Rational(l.m_numerator - r.m_numerator, l.m_denominator);
  } else if((l.m_denominator > r.m_denominator) && (l.m_denominator % r.m_denominator == 0)) { // l.d = n * r.d. extend r with n and use l.d as denominator
    const INT64 n = l.m_denominator / r.m_denominator;
    return Rational(l.m_numerator - SAFEPROD(n,r.m_numerator), l.m_denominator);
  } else if((l.m_denominator < r.m_denominator) && (r.m_denominator % l.m_denominator == 0)) { // r.d = n * l.d. extend l with n and use r.d as denominator
    const INT64 n = r.m_denominator / l.m_denominator;
    return Rational(SAFEPROD(n,l.m_numerator) - r.m_numerator, r.m_denominator);
  } else {                                                                                     // Extend both and use l.d * r.d as denominator
    return Rational(SAFEPROD(l.m_numerator,r.m_denominator) - SAFEPROD(r.m_numerator,l.m_denominator), SAFEPROD(l.m_denominator,r.m_denominator));
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
    return Rational(-r.m_numerator, r.m_denominator);
  default             :
    return r;
  }
}

Rational operator*(const Rational &l, const Rational &r) {
  if(!isfinite(l) || !isfinite(r)) return RAT_NAN;
  return Rational(SAFEPROD(l.m_numerator,r.m_numerator), SAFEPROD(l.m_denominator,r.m_denominator));
}

void Rational::throwDivisionByZeroException(const TCHAR *method) { // static
  throwInvalidArgumentException(method, _T("Division by zero"));
}

Rational operator/(const Rational &l, const Rational &r) {
  const int lclass = _fpclass(l);
  if((lclass & (_FPCLASS_NN | _FPCLASS_PN | _FPCLASS_NZ | _FPCLASS_PZ)) == 0) {
    return Rational::_RAT_NAN;
  }
  switch(fpclassify(r)) {
  case FP_NORMAL:
    return Rational(SAFEPROD(l.m_numerator, r.m_denominator), SAFEPROD(l.m_denominator, r.m_numerator));
  case FP_ZERO:
    switch(lclass) {
    case _FPCLASS_PN: return Rational::_RAT_PINF;   // +finite/0 -> +inf
    case _FPCLASS_NN: return Rational::_RAT_NINF;   // -finite/0 -> -inf
    case _FPCLASS_NZ:
    case _FPCLASS_PZ: return Rational::_RAT_NAN;   // 0/0       ->  nan
    }
  }
  return Rational::_RAT_NAN;
}

Rational operator%(const Rational &l, const Rational &r) {
  if(!isfinite(l) || !isfinite(r)) return RAT_NAN;
  if(r.isZero()) {
    if(l.isZero()    ) return RAT_NAN;
    if(l.isNegative()) return RAT_NINF;
    return RAT_PINF;
  }
  const Rational q = l / r;
  const INT64    n = getInt64(q);
  return l - r * n;
}

Rational fabs(const Rational &r) {
  if(isnan(r)) return RAT_NAN;
  Rational result;
  result.m_numerator   = abs(r.m_numerator);
  result.m_denominator = r.m_denominator;
  return result;
}

INT64 Rational::pow(INT64 n, UINT y) {
  INT64 p = 1;
  while(y > 0) {
    if(::isEven(y)) {
      n = SAFEPROD(n,n);
      y /= 2;
    } else {
      p = SAFEPROD(p,n);
      y--;
    }
  }
  return p;
}

Rational pow(const Rational &r, int e) {
  if(!isfinite(r)) return RAT_NAN;
  if(e == 0) {
    return Rational::_1;
  } else if(e < 0) {
    if(r.isZero()) {
      return RAT_PINF;
    }
    return Rational(Rational::pow(r.m_denominator,-e), Rational::pow(r.m_numerator,-e));
  } else {
    return Rational(Rational::pow(r.m_numerator,e), Rational::pow(r.m_denominator,e));
  }
}

Rational reciprocal(const Rational &r) {
  switch(fpclassify(r)) {
  case FP_NORMAL   : return Rational(r.m_denominator, r.m_numerator);
  case FP_ZERO     : return Rational::_RAT_PINF;
  case FP_INFINITE : return Rational::_RAT_NAN;
  case FP_NAN      : return r;
  default          : throwException(_T("%s:Unknown value return from fpclassify:%d"), __TFUNCTION__, fpclassify(r));
                     return Rational::_0;
  }
}

int rationalCmp(const Rational &r1, const Rational &r2) {
  assert(isfinite(r1) && isfinite(r2));
  const int sign1 = sign(r1.m_numerator);
  const int c     = sign1 - sign(r2.m_numerator);
  if(c != 0) return c;
  if(abs(r1.m_numerator)      > (UINT)UINT_MAX
  || abs(r2.m_numerator)      > (UINT)UINT_MAX
  || (UINT64)r1.m_denominator > (UINT)UINT_MAX
  || (UINT64)r2.m_denominator > (UINT)UINT_MAX) {
    return sign(getDouble(r1) - getDouble(r2));
  } else {
    const UINT64 p1 = (UINT64)abs(r1.m_numerator) * r2.m_denominator;
    const UINT64 p2 = (UINT64)abs(r2.m_numerator) * r1.m_denominator;
    return (p1 > p2) ? sign1 : (p1 < p2) ? -sign1 : 0;
  }
}

INT64 Rational::safeProd(const INT64 &a, const INT64 &b, int line) { // static
  if((a|b) == 0) return 0;
  _int128 result(a);
  result *= b;
  if(result > _I64_MAX || result < _I64_MIN) {
    throwInvalidArgumentException(__TFUNCTION__,_T("%s line %d: Product overflow. a=%I64d, b=%I64d"), __TFILE__, line, a, b);
  }
  return result;
}

UINT64 Rational::findGCD(const UINT64 &a, const UINT64 &b) { // static
  if(a == 0 || b == 0) {
    throwInvalidArgumentException(__TFUNCTION__, _T("a=%I64u, b=%I64u"), a, b);
  }
  UINT64 g = 1;
  UINT64 u = a;
  UINT64 v(b);

  while(::isEven(u) && ::isEven(v)) {
    u /= 2;
    v /= 2;
    g *= 2;
  }

  // Now u or v (or both) are odd
  while(u > 0) {
    if(::isEven(u)) {
      u /= 2;
    } else if(::isEven(v)) {
      v /= 2;
    } else if(u < v) {
      v = (v-u)/2;
    } else {
      u = (u-v)/2;
    }
  }
  return g*v;
}

#define CHECKISFINITE1(f)                  \
if(!isfinite(f)) {                         \
  if(r) {                                  \
    if(isPInfinity(f))      *r = _RAT_PINF; \
    else if(isNInfinity(f)) *r = _RAT_NINF; \
    else                    *r = _RAT_NAN;  \
  }                                        \
  return true;                             \
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
