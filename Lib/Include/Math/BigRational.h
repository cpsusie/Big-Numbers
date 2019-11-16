#pragma once

#include "BigInt.h"
#include "Rational.h"

class ConstBigRational;

class BigRational {
private:
  inline void modifyFlags(BYTE add, BYTE remove) {
    m_numerator.modifyFlags(  add, remove);
    m_denominator.modifyFlags(add, remove);
  }
protected:
  BigInt m_numerator, m_denominator;
  // Assume a._normal() && b._isnormal() && a > 0 && b > 0
  static BigInt findGCD(const BigInt &a, const BigInt &b, DigitPool *pool);
  void init(const BigInt &numerator, const BigInt &denominator);
  void init(const String &s);

  // Set this to corresponding non-normal value, and return *this
  // Assume fpclass is one of {_FPCLASS_PZ,_FPCLASS_NZ, _FPCLASS_PINF, _FPCLASS_NINF, _FPCLASS_QNAN, _FPCLASS_SNAN }
  // throws exception if fpclass is not in the list above
  BigRational &setToNonNormalFpClass(int fpclass);

  virtual bool allowConstDigitPool() const {
    return false;
  }
  inline BigRational &clrInitDone() {
    modifyFlags(0, BR_INITDONE);
    return *this;
  }
  inline BigRational &setInitDone() {
    modifyFlags(BR_INITDONE, 0);
    return *this;
  }
public:
  BigRational(                                                              DigitPool *digitPool = NULL);
  BigRational(const BigRational      &r                                   , DigitPool *digitPool = NULL);
  BigRational(const BigInt           &numerator, const BigInt &denominator, DigitPool *digitPool = NULL);
  BigRational(const BigInt           &n                                   , DigitPool *digitPool = NULL);
  BigRational(int                     n                                   , DigitPool *digitPool = NULL);
  BigRational(UINT                    n                                   , DigitPool *digitPool = NULL);
  BigRational(long                    n                                   , DigitPool *digitPool = NULL);
  BigRational(ULONG                   n                                   , DigitPool *digitPool = NULL);
  BigRational(INT64                   n                                   , DigitPool *digitPool = NULL);
  BigRational(UINT64                  n                                   , DigitPool *digitPool = NULL);
  BigRational(const _int128          &n                                   , DigitPool *digitPool = NULL);
  BigRational(const _uint128         &n                                   , DigitPool *digitPool = NULL);
  BigRational(const Rational         &r                                   , DigitPool *digitPool = NULL);
  explicit BigRational(const String  &s                                   , DigitPool *digitPool = NULL);
  explicit BigRational(const char    *s                                   , DigitPool *digitPool = NULL);
  explicit BigRational(const wchar_t *s                                   , DigitPool *digitPool = NULL);

  friend BigRational operator+(const BigRational &l, const BigRational &r);
  friend BigRational operator-(const BigRational &l, const BigRational &r);
  friend BigRational operator-(const BigRational &r);
  friend BigRational operator*(const BigRational &l, const BigRational &r);
  friend BigRational operator/(const BigRational &l, const BigRational &r);

  BigRational &operator+=(const BigRational &r);
  BigRational &operator-=(const BigRational &r);
  BigRational &operator*=(const BigRational &r);
  BigRational &operator/=(const BigRational &r);

  // Assume r1._isfinite() && r2._isfinite()
  static int compare(const BigRational &r1, const BigRational &r2);

  inline const BigInt &getNumerator() const {
    return m_numerator;
  }

  inline const BigInt &getDenominator() const {
    return m_denominator;
  }

  inline DigitPool *getDigitPool() const {
    return m_denominator.getDigitPool();
  }
  inline BYTE getFlags() const {
    return m_numerator.getFlags();
  }
  inline bool _isnormal() const {
    return m_numerator._isnormal() && m_denominator._isnormal();
  }
  // Return true if *this == 0 (0/1)
  inline  bool isZero() const {
    return m_numerator.isZero() && (m_denominator == BigReal::_1);
  }
  // Return true, if *this is +/-INFINITE...+inf=(1/0), -inf=(-1/0)
  inline bool _isinf() const {
    return m_denominator.isZero() && m_numerator._isnormal();
  }
  // Return true, if _isnormal() || isZero() <=> denominator._isnormal()
  inline bool _isfinite() const {
    return m_denominator._isnormal();
  }
  // Return true, if *this is NaN (=(0/0))
  inline bool _isnan() const {
    return m_denominator.isZero() && m_numerator.isZero();
  }
  // Return getNumerator().isPositive()
  inline  bool isPositive() const {
    return m_numerator.isPositive();
  }
  // Return getNumerator().isNegative()
  inline bool isNegative() const {
    return m_numerator.isNegative();
  }
  inline bool isInteger() const {
    return m_denominator == BigReal::_1;
  }
  BigRational &setToZero();
  BigRational &setToPInf();
  BigRational &setToNInf();
  BigRational &setToNan();

  static const ConstBigRational _0;          // 0
  static const ConstBigRational _05;         // 1/2
  static const ConstBigRational _1;          // 1
  static const ConstBigRational _2;          // 2
  static const ConstBigRational _BRAT_QNAN;  // non-signaling NaN (quiet NaN)
  static const ConstBigRational _BRAT_PINF;  // +infinity;
  static const ConstBigRational _BRAT_NINF;  // -infinity;

  void assertIsValid() const;
  inline String flagsToString() const {
    return BigReal::flagsToString(getFlags());
  }
};

class ConstBigRational : public BigRational {
protected:
  bool allowConstDigitPool() const {
    return true;
  }

public:
  inline ConstBigRational(const BigRational &r)                                    : BigRational(r                     , CONST_DIGITPOOL) {
  }
  inline ConstBigRational(const BigInt      &numerator, const BigInt &denominator) : BigRational(numerator, denominator, CONST_DIGITPOOL) {
  }
  inline ConstBigRational(const BigInt      &n                                   ) : BigRational(n                     , CONST_DIGITPOOL) {
  }
  inline ConstBigRational(int                n                                   ) : BigRational(n                     , CONST_DIGITPOOL) {
  }
  inline ConstBigRational(UINT               n                                   ) : BigRational(n                     , CONST_DIGITPOOL) {
  }
  inline ConstBigRational(long               n                                   ) : BigRational(n                     , CONST_DIGITPOOL) {
  }
  inline ConstBigRational(ULONG              n                                   ) : BigRational(n                     , CONST_DIGITPOOL) {
  }
  inline ConstBigRational(INT64              n                                   ) : BigRational(n                     , CONST_DIGITPOOL) {
  }
  inline ConstBigRational(UINT64             n                                   ) : BigRational(n                     , CONST_DIGITPOOL) {
  }
  inline ConstBigRational(const _int128     &n                                   ) : BigRational(n                     , CONST_DIGITPOOL) {
  }
  inline ConstBigRational(const _uint128    &n                                   ) : BigRational(n                     , CONST_DIGITPOOL) {
  }
  inline ConstBigRational(const Rational    &r                                   ) : BigRational(r                     , CONST_DIGITPOOL) {
  }
  explicit ConstBigRational(const String    &s                                   ) : BigRational(s                     , CONST_DIGITPOOL) {
  }
  explicit ConstBigRational(const char      *s                                   ) : BigRational(s                     , CONST_DIGITPOOL) {
  }
  explicit ConstBigRational(const wchar_t   *s                                   ) : BigRational(s                     , CONST_DIGITPOOL) {
  }
};

// Assume r._isfinite(). Return r < 0 ? -1 : r > 0 ? 1 : 0
inline int sign(const BigRational &r) {
  assert(r._isfinite());
  return r._isnormal() ? r.isNegative() ? -1 : 1 : 0;
}

// Return uniform distributed random rational in range [0;1[ using rnd to generate numerator and denominator
// The denominator (den) will be uniform distributed in the range [2; maxDenominator]
// The numerator will be uniform distributed in range [0; den[
// If digitPool == null, returned value will use maxDenominator.getDigitPool()
// Assume maxDenominator >= 2
BigRational randBigRational(const BigInt &maxDenominator, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator, DigitPool *digitPool = NULL);

// Return uniform distributed random BigRational in range [from;to] (both inclusive)
// First make the the 3 products: n1 = from.num*to.den, n2 = to.num*from.den, d = from.den*to.den
// The returned rational will have the value rn/(d*f), where rn is a random BigInt in the range [n1*f..n2*f],
// where f is a random BigInt in range [1..maxScaleFactor]
// If maxScaleFactor <= 1, no scaling is done
// If digitPool == null, returned value will use from.getDigitPool()
BigRational randBigRational(const BigRational &from, const BigRational &to, const BigInt &maxScaleFactor, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator, DigitPool *digitPool = NULL);

// returns one of
// FP_INFINITE
// FP_NAN
// FP_NORMAL
// FP_ZERO
inline int fpclassify(const BigRational &r) {
  if(!r.getDenominator().isZero()) return r.getNumerator().isZero() ? FP_ZERO : FP_NORMAL;
  return r.getNumerator().isZero() ? FP_NAN : FP_INFINITE;
}

// returns one of
// _FPCLASS_QNAN  0x0002   quiet NaN
// _FPCLASS_NINF  0x0004   negative infinity
// _FPCLASS_NN    0x0008   negative normal
// _FPCLASS_PZ    0x0040   +0
// _FPCLASS_PN    0x0100   positive normal
// _FPCLASS_PINF  0x0200   positive infinity
int _fpclass(const BigRational &r);

inline bool isfinite(const BigRational &r) {
  return fpclassify(r) <= 0;
}
inline bool isinf(const BigRational &r) {
  return fpclassify(r) == FP_INFINITE;
}
inline bool isnan(const BigRational &r) {
  return fpclassify(r) == FP_NAN;
}
inline bool isnormal(const BigRational &r) {
  return fpclassify(r) == FP_NORMAL;
}
inline bool isunordered(const BigRational &x, const BigRational &y) {
  return isnan(x) || isnan(y);
}
inline bool isPInfinity(const BigRational &r) {
  return isinf(r) && r.isPositive();
}
inline bool isNInfinity(const BigRational &r) {
  return isinf(r) && r.isNegative();
}

inline bool operator< (const BigRational &x, const BigRational &y) {
  return !isunordered(x, y) && (BigRational::compare(x, y) < 0);
}
inline bool operator> (const BigRational &x, const BigRational &y) {
  return !isunordered(x, y) && (BigRational::compare(x, y) > 0);
}
inline bool operator<=(const BigRational &x, const BigRational &y) {
  return !isunordered(x, y) && (BigRational::compare(x, y) <= 0);
}
inline bool operator>=(const BigRational &x, const BigRational &y) {
  return !isunordered(x, y) && (BigRational::compare(x, y) >= 0);
}
inline bool operator==(const BigRational &x, const BigRational &y) {
  return !isunordered(x, y) && ((x.getNumerator() == y.getNumerator()) && (x.getDenominator() == y.getDenominator()));
}
inline bool operator!=(const BigRational &x, const BigRational &y) {
  return !isunordered(x, y) && ((x.getNumerator() != y.getNumerator()) || (x.getDenominator() != y.getDenominator()));
}

std::istream     &operator>>(std::istream  &in ,       BigRational       &r);
std::ostream     &operator<<(std::ostream  &out, const BigRational       &r);
std::wistream    &operator>>(std::wistream &in,        BigRational       &r);
std::wostream    &operator<<(std::wostream &out, const BigRational       &r);

String      toString(const BigRational       &r, StreamSize width = 0, FormatFlags flags = 0, TCHAR separatorChar = 0);
BigRational inputRational(DigitPool &digitPool, _In_z_ _Printf_format_string_ TCHAR const * const format, ...);
