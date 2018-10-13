#pragma once

#include <StrStream.h>
#include <Math/Double80.h>
#include <Math/Real.h>

class Rational {
private:
  INT64 m_numerator, m_denominator;

  void init(const INT64 &numerator, const INT64 &denominator);
  static void throwDivisionByZeroException(const TCHAR *method);
  static INT64 pow(INT64 n, UINT y);
public:
  inline Rational() : m_numerator(0), m_denominator(1) {
  }
  inline Rational(const INT64 &numerator, const INT64 &denominator) {
    init(numerator, denominator);
  }
  inline Rational(const INT64 &numerator, int denominator) {
    init(numerator, denominator);
  }
  inline Rational(int numerator, int denominator) {
    init(numerator, denominator);
  }
  inline Rational(int numerator, const INT64 &denominator) {
    init(numerator, denominator);
  }
  inline Rational(const INT64 &n) : m_numerator(n), m_denominator(1) {
  }
  inline Rational(int n) : m_numerator(n), m_denominator(1) {
  }
  inline Rational(UINT n) : m_numerator(n), m_denominator(1) {
  }
  explicit Rational(float           f  , UINT   maxND = _I16_MIN);
  explicit Rational(double          d  , UINT   maxND = _I32_MIN);
  explicit Rational(const Double80 &d80, UINT64 maxND = _I64_MAX);

  static INT64 safeProd(const INT64 &a, const INT64 &b, int line);

  friend Rational operator+(const Rational &l, const Rational &r);
  friend Rational operator-(const Rational &l, const Rational &r);
  friend Rational operator-(const Rational &r);
  friend Rational operator*(const Rational &l, const Rational &r);
  friend Rational operator/(const Rational &l, const Rational &r);
  friend Rational operator%(const Rational &l, const Rational &r);

  inline Rational &operator+=(const Rational &r) {
    return *this = *this + r;
  }

  inline Rational &operator-=(const Rational &r) {
    return *this = *this - r;
  }

  Rational &operator*=(const Rational &r);
  Rational &operator/=(const Rational &r);
  Rational &operator%=(const Rational &r);

  // Assume isfinite(r1) && isfinite(r2)
  friend int rationalCmp(const Rational &r1, const Rational &r2);

  friend Rational fabs(const Rational &r);
  friend Rational pow( const Rational &r, int e);
  friend Rational reciprocal(const Rational &r);

  inline bool isZero() const {
    return (m_numerator == 0) && (m_denominator == 1);
  }
  inline bool isNegative() const {
    return m_numerator < 0;
  }
  inline bool isPositive() const {
    return m_numerator > 0;
  }
  inline bool isInteger() const {
    return m_denominator == 1;
  }
  static UINT64 findGCD(const UINT64 &a, const UINT64 &b);

  inline const INT64 &getNumerator() const {
    return m_numerator;
  }

  inline const INT64 &getDenominator() const {
    return m_denominator;
  }
  // Return true if x is a rational with denominator<=400.
  // if(r != NULL), *r will contain converted Rational
  static bool isRational(float           x, Rational *r);
  static bool isRational(double          x, Rational *r);
  static bool isRational(const Double80 &x, Rational *r);
  // Return true if base^e is a rational
  // if(r != NULL), *r will contain the calculated value
  static bool isRationalPow(const Rational &base, const Rational &e, Rational *r);

  inline ULONG hashCode() const {
    return int64Hash(m_numerator) + 100999 * int64Hash(m_denominator);
  }

  inline void save(ByteOutputStream &s) const {
    s.putBytes((BYTE*)this, sizeof(Rational));
  }
  inline void load(ByteInputStream  &s) {
    s.getBytesForced((BYTE*)this, sizeof(Rational));
  }

  friend inline Packer &operator<<(Packer &p, const Rational &r) {
    return p << r.m_numerator << r.m_denominator;
  }
  friend inline Packer &operator>>(Packer &p, Rational &r) {
    return p >> r.m_numerator >> r.m_denominator;
  }
};

extern const Rational RAT_MIN;     // Min positive value (=1/_I64_MAX)
extern const Rational RAT_MAX;     // Max value          (=_I64_MAX)
extern const Rational RAT_NAN;     // nan (undefined)    ( 0/0)
extern const Rational RAT_PINF;    // +infinity;         ( 1/0)
extern const Rational RAT_NINF;    // -infinity;         (-1/0)

inline bool isInteger(const Rational &r) {
  return r.getDenominator() == 1;
}
inline bool isInt(const Rational &r) {
  return r.isInteger() && (_I32_MIN <= r.getNumerator()) && (r.getNumerator() <= _I32_MAX);
}
inline INT64    getInt64(   const Rational &r) {
  return r.getNumerator() / r.getDenominator();
}
inline UINT64   getUint64(  const Rational &r) {
  return (UINT64)r.getNumerator() / r.getDenominator();
}
inline int      getInt(     const Rational &r) {
  return (int)getInt64(r);
}
inline UINT     getUint(    const Rational &r) {
  return (UINT)getUint64(r);
}
inline long     getLong(    const Rational &r) {
  return getInt(r);
}
inline ULONG    getUlong(   const Rational &r) {
  return getUint(r);
}
inline float    getFloat(   const Rational &r) {
  return (float)((double)r.getNumerator()/r.getDenominator());
}
inline double   getDouble(  const Rational &r) {
  return (double)r.getNumerator()/r.getDenominator();
}
inline Double80 getDouble80(const Rational &r) {
  return Double80(r.getNumerator())/r.getDenominator();
}

inline Real     getReal(    const Rational &r) {
#ifdef LONGDOUBLE
  return getDouble80(r);
#else
  return getDouble(r);
#endif
}

inline bool isEven(const Rational &r) {
  return isInteger(r) && isEven(r.getNumerator());
}
inline bool isOdd(const Rational &r) {
  return isInteger(r) && isOdd(r.getNumerator());
}
inline bool isSymmetricExponent(const Rational &r) {
  return isEven(r.getNumerator()) || isEven(r.getDenominator());
}

inline bool isAsymmetricExponent(const Rational &r) {
  return isOdd(r.getNumerator()) && isOdd(r.getDenominator());
}

inline bool rationalExponentsMultiply(const Rational &r1, const Rational &r2) {
  return isAsymmetricExponent(r1) || isAsymmetricExponent(r2);
}

inline int fpclassify(const Rational &r) {
  if(r.getDenominator()) return r.getNumerator() ? FP_NORMAL : FP_ZERO;
  return r.getNumerator() ? FP_INFINITE : FP_NAN;
}

inline bool isfinite(const Rational &r) {
  return fpclassify(r) <= 0;
}
inline bool isinf(const Rational &r) {
  return fpclassify(r) == FP_INFINITE;
}
inline bool isnan(const Rational &r) {
  return fpclassify(r) == FP_NAN;
}
inline bool isnormal(const Rational &r) {
  return fpclassify(r) == FP_NORMAL;
}
inline bool isunordered(const Rational &x, const Rational &y) {
  return isnan(x) || isnan(y);
}
inline bool isPInfinity(const Rational &r) {
  return isinf(r) && r.isPositive();
}
inline bool isNInfinity(const Rational &r) {
  return isinf(r) && r.isNegative();
}

inline bool operator< (const Rational &x, const Rational &y) {
  return !isunordered(x,y) && (rationalCmp(x, y) < 0);
}
inline bool operator> (const Rational &x, const Rational &y) {
  return !isunordered(x,y) && (rationalCmp(x, y) > 0);
}
inline bool operator<=(const Rational &x, const Rational &y) {
  return !isunordered(x,y) && (rationalCmp(x, y) <= 0);
}
inline bool operator>=(const Rational &x, const Rational &y) {
  return !isunordered(x,y) && (rationalCmp(x, y) >= 0);
}
inline bool operator==(const Rational &x, const Rational &y) {
  return !isunordered(x,y) && ((x.getNumerator() == y.getNumerator()) && (x.getDenominator() == y.getDenominator()));
}
inline bool operator!=(const Rational &x, const Rational &y) {
  return !isunordered(x,y) && ((x.getNumerator() != y.getNumerator()) || (x.getDenominator() != y.getDenominator()));
}


char    *rattoa(char    *dst, const Rational &r, int radix);
wchar_t *rattow(wchar_t *dst, const Rational &r, int radix);

#ifdef _UNICODE
#define _rattot rattow
#else
#define _rattot rattoa
#endif

String toString(const Rational &r, int precision=0, int width=0, int flags=0);

Rational strtorat(const char *s   , char    **end, int radix);
Rational wcstorat(const wchar_t *s, wchar_t **end, int radix);

#ifdef _UNICODE
#define _tcstorat wcstorat
#else
#define _tcstorat strtorat
#endif // _UNICODE

istream &operator>>(istream &s,       Rational &r);
ostream &operator<<(ostream &s, const Rational &r);

std::wistream &operator>>(std::wistream &s,       Rational &r);
std::wostream &operator<<(std::wostream &s, const Rational &r);

StrStream &operator<<(StrStream &stream, const Rational &r);
