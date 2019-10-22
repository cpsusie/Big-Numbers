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

};

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

std::istream     &operator>>(std::istream  &in ,       BigRational       &x);
std::ostream     &operator<<(std::ostream  &out, const BigRational       &x);
std::wistream    &operator>>(std::wistream &in,        BigRational       &x);
std::wostream    &operator<<(std::wostream &out, const BigRational       &x);

String      toString(const BigRational       &x, StreamSize width = 0, FormatFlags flags = 0, TCHAR separatorChar = 0);
BigRational inputRational(DigitPool &digitPool, _In_z_ _Printf_format_string_ TCHAR const * const format, ...);
