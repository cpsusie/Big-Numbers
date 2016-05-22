#pragma once

#include <StrStream.h>
#include <Math/Double80.h>
#include <Math/Real.h>

class Rational {
#ifdef _DEBUG
private:
  String                m_debugString;
  static bool           s_debugStringEnabled;
  inline void initDebugString() {
    m_debugString = toString();
  }
public:
  const String &getDebugString() const { return m_debugString; }

#define INITRATIONALDEBUGSTRING(r) { if(Rational::s_debugStringEnabled) (r).initDebugString(); }
#else
#define INITRATIONALDEBUGSTRING(r)
#endif

private:
  DECLARECLASSNAME;
  __int64 m_numerator, m_denominator;

  static __int64 findGCD(const __int64 &a, const __int64 &b);

  void init(const __int64 &numerator, const __int64 &denominator);
  void init(const String &s);
  static void throwDivisionbyZeroException(const TCHAR *method);
  static __int64 pow(__int64 n, int y); // assume y >= 0
public:
  Rational();
  Rational(const __int64 &numerator, const __int64 &denominator);
  Rational(const __int64 &numerator, int            denominator);
  Rational(int            numerator, int            denominator);
  Rational(int            numerator, const __int64 &denominator);
  Rational(const __int64           &n  );
  Rational(int                      n  );
  Rational(unsigned int             n  );
  explicit Rational(double          d  , unsigned int     maxND = _I32_MIN);
  explicit Rational(const Double80 &d80, unsigned __int64 maxND = _I64_MAX);
  explicit Rational(const String   &s  );
  explicit Rational(const TCHAR    *s  );
#ifdef UNICODE
  explicit Rational(const char     *s  );
#endif

  static __int64 safeProd(const __int64 &a, const __int64 &b, int line);

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

  bool operator< (const Rational &r) const;
  bool operator> (const Rational &r) const;
  bool operator<=(const Rational &r) const;
  bool operator>=(const Rational &r) const;
  bool operator==(const Rational &r) const;
  bool operator!=(const Rational &r) const;

  friend Rational fabs(const Rational &r);
  friend Rational pow( const Rational &r, int e);
  friend Rational reciprocal(const Rational &r);

  inline friend   int     getInt(     const Rational &r) {
    return getLong(r);
  }
  inline friend unsigned int getUint( const Rational &r) {
    return getUlong(r);
  }
  friend          long    getLong(    const Rational &r);
  friend unsigned long    getUlong(   const Rational &r);
  friend          __int64 getInt64(   const Rational &r);
  friend unsigned __int64 getUint64(  const Rational &r);

  friend float            getFloat(   const Rational &r) {
    return (float)((double)r.m_numerator/r.m_denominator);
  }

  friend inline double    getDouble(  const Rational &r) {
    return (double)r.m_numerator/r.m_denominator;
  }

  friend inline Double80  getDouble80(const Rational &r) {
    return Double80(r.m_numerator)/Double80(r.m_denominator);
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
  inline const __int64 &getNumerator() const {
    return m_numerator;
  }

  inline const __int64 &getDenominator() const {
    return m_denominator;
  }

  static bool isRealRational(const Real &x, Rational *r);

  inline unsigned long hashCode() const {
    return int64Hash(m_numerator) + 100999 * int64Hash(m_denominator);
  }

  String toString() const;

  inline void save(ByteOutputStream &s) const {
    s.putBytes((BYTE*)this, sizeof(Rational));
  }
  inline void load(ByteInputStream  &s) {
    s.getBytesForced((BYTE*)this, sizeof(Rational));
    INITRATIONALDEBUGSTRING(*this);
  }

  friend inline Packer &operator<<(Packer &p, const Rational &r) {
    return p << r.m_numerator << r.m_denominator;
  }
  friend inline Packer &operator>>(Packer &p, Rational &r) {
#ifdef _DEBUG
    p >> r.m_numerator >> r.m_denominator;
    INITRATIONALDEBUGSTRING(r);
    return p;
#else
    return p >> r.m_numerator >> r.m_denominator;
#endif
  }
  static bool enableDebugString(bool enabled);

};

String toString(const Rational &r, int precision=0, int width=0, int flags=0);

tistream &operator>>(tistream &s,       Rational &r);
tostream &operator<<(tostream &s, const Rational &r);

StrStream &operator<<(StrStream &stream, const Rational &r);
