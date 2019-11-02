#pragma once

#include "BigInt.h"

class BigRational {
private:
  BigInt m_numerator, m_denominator;
  static BigInt findGCD(const BigInt &a, const BigInt &b);
  void init(const BigInt &numerator, const BigInt &denominator);
  void init(const String &s);
public:
  BigRational(DigitPool *digitPool = NULL);
  BigRational(const BigInt &numerator, const BigInt &denominator, DigitPool *digitPool = NULL);
  BigRational(const BigInt &n, DigitPool *digitPool = NULL);
  BigRational(int           n, DigitPool *digitPool = NULL);
  explicit BigRational(const String  &s, DigitPool *digitPool = NULL);
  explicit BigRational(const char    *s, DigitPool *digitPool = NULL);
  explicit BigRational(const wchar_t *s, DigitPool *digitPool = NULL);

  friend BigRational operator+(const BigRational &l, const BigRational &r);
  friend BigRational operator-(const BigRational &l, const BigRational &r);
  friend BigRational operator-(const BigRational &r);
  friend BigRational operator*(const BigRational &l, const BigRational &r);
  friend BigRational operator/(const BigRational &l, const BigRational &r);

  BigRational &operator+=(const BigRational &r);
  BigRational &operator-=(const BigRational &r);
  BigRational &operator*=(const BigRational &r);
  BigRational &operator/=(const BigRational &r);

  // Assume isfinite(r1) && isfinite(r2)
  friend int bigRationalCmp(const BigRational &r1, const BigRational &r2);

  const BigInt &getNumerator() const;
  const BigInt &getDenominator() const;

  DigitPool *getDigitPool() const {
    return m_denominator.getDigitPool();
  }
  inline  bool isZero() const {
    return getNumerator().isZero() && !getDenominator().isZero();
  }
  inline  bool isPositive() const {
    return getNumerator().isPositive();
  }
  // Return m_negative
  inline bool isNegative() const {
    return getNumerator().isNegative();
  }
  inline bool isInteger() const {
    return m_denominator == BigReal::_1;
  }

  static const BigRational _0;          // 0
  static const BigRational _05;         // 1/2
  static const BigRational _1;          // 1
  static const BigRational _2;          // 2
  static const BigRational _BRAT_QNAN;  // non-signaling NaN (quiet NaN)
  static const BigRational _BRAT_PINF;  // +infinity;
  static const BigRational _BRAT_NINF;  // -infinity;
};

int sign(const BigRational &r);

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
// FP_SUBNORMAL .... NOT used
// FP_ZERO
inline int fpclassify(const BigRational &r) {
  if(!r.getDenominator().isZero()) return r.getNumerator().isZero() ? FP_ZERO : FP_NORMAL;
  return r.getNumerator().isZero() ? FP_NAN : FP_INFINITE;
}

// returns one of
// _FPCLASS_SNAN  0x0001   signaling NaN     .... NOT used
// _FPCLASS_QNAN  0x0002   quiet NaN
// _FPCLASS_NINF  0x0004   negative infinity
// _FPCLASS_NN    0x0008   negative normal
// _FPCLASS_ND    0x0010   negative denormal .... NOT used
// _FPCLASS_NZ    0x0020   -0                .... NOT used
// _FPCLASS_PZ    0x0040   +0
// _FPCLASS_PD    0x0080   positive denormal .... NOT used
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
  return !isunordered(x, y) && (bigRationalCmp(x, y) < 0);
}
inline bool operator> (const BigRational &x, const BigRational &y) {
  return !isunordered(x, y) && (bigRationalCmp(x, y) > 0);
}
inline bool operator<=(const BigRational &x, const BigRational &y) {
  return !isunordered(x, y) && (bigRationalCmp(x, y) <= 0);
}
inline bool operator>=(const BigRational &x, const BigRational &y) {
  return !isunordered(x, y) && (bigRationalCmp(x, y) >= 0);
}
inline bool operator==(const BigRational &x, const BigRational &y) {
  return !isunordered(x, y) && ((x.getNumerator() == y.getNumerator()) && (x.getDenominator() == y.getDenominator()));
}
inline bool operator!=(const BigRational &x, const BigRational &y) {
  return !isunordered(x, y) && ((x.getNumerator() != y.getNumerator()) || (x.getDenominator() != y.getDenominator()));
}

std::istream     &operator>>(std::istream  &in ,       BigRational       &x);
std::ostream     &operator<<(std::ostream  &out, const BigRational       &x);
std::wistream    &operator>>(std::wistream &in,        BigRational       &x);
std::wostream    &operator<<(std::wostream &out, const BigRational       &x);

String      toString(const BigRational       &x, StreamSize width = 0, FormatFlags flags = 0, TCHAR separatorChar = 0);
BigRational inputRational(DigitPool &digitPool, _In_z_ _Printf_format_string_ TCHAR const * const format, ...);
