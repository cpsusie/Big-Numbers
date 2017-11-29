#pragma once

#include <StrStream.h>
#include <Math/Double80.h>
#include <Math/Real.h>

class Rational {
private:
  INT64 m_numerator, m_denominator;

  void init(const INT64 &numerator, const INT64 &denominator);
  static void throwDivisionbyZeroException(const TCHAR *method);
  static INT64 pow(INT64 n, int y); // assume y >= 0
public:
  Rational();
  Rational(const INT64 &numerator, const INT64 &denominator);
  Rational(const INT64 &numerator, int          denominator);
  Rational(int          numerator, int          denominator);
  Rational(int          numerator, const INT64 &denominator);
  Rational(const INT64             &n  );
  Rational(int                      n  );
  Rational(UINT                     n  );
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

  friend int rationalCmp(const Rational &r1, const Rational &r2);

  inline bool operator< (const Rational &r) const {
    return rationalCmp(*this, r) < 0;
  }
  inline bool operator> (const Rational &r) const {
    return rationalCmp(*this, r) > 0;
  }
  inline bool operator<=(const Rational &r) const {
    return rationalCmp(*this, r) <= 0;
  }
  inline bool operator>=(const Rational &r) const {
    return rationalCmp(*this, r) >= 0;
  }
  inline bool operator==(const Rational &r) const {
    return (m_numerator == r.m_numerator) && (m_denominator == r.m_denominator);
  }
  inline bool operator!=(const Rational &r) const {
    return !(*this == r);
  }

  friend Rational fabs(const Rational &r);
  friend Rational pow( const Rational &r, int e);
  friend Rational reciprocal(const Rational &r);

  inline friend   int     getInt(     const Rational &r) {
    return getLong(r);
  }
  inline friend UINT getUint( const Rational &r) {
    return getUlong(r);
  }
  friend long   getLong(    const Rational &r);
  friend ULONG  getUlong(   const Rational &r);
  friend INT64  getInt64(   const Rational &r);
  friend UINT64 getUint64(  const Rational &r);

  friend inline float     getFloat(   const Rational &r) {
    return (float)((double)r.m_numerator/r.m_denominator);
  }

  friend inline double    getDouble(  const Rational &r) {
    return (double)r.m_numerator/r.m_denominator;
  }

  friend inline Double80  getDouble80(const Rational &r) {
    return Double80(r.m_numerator)/r.m_denominator;
  }

  friend inline Real      getReal(     const Rational &r) {
#ifdef LONGDOUBLE
    return getDouble80(r);
#else
    return getDouble(r);
#endif
  }

  inline bool isZero() const {
    return m_numerator == 0;
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
  inline bool isEven() const {
    return isInteger() && ((m_numerator & 1) == 0);
  }
  inline bool isOdd() const {
    return isInteger() && ((m_numerator & 1) == 1);
  }
  inline bool isInt32() const {
    return isInteger() && (_I32_MIN <= m_numerator) && (m_numerator <= _I32_MAX);
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

template<class T> bool isOdd(T x) {
  return x & 1;
}

template<class T> bool isEven(T x) {
  return (x & 1) == 0;
}

inline bool isSymmetricExponent(const Rational &r) {
  return ::isEven(r.getNumerator()) || ::isEven(r.getDenominator());
}

inline bool isAsymmetricExponent(const Rational &r) {
  return isOdd(r.getNumerator()) && isOdd(r.getDenominator());
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
