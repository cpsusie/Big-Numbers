#pragma once

#include "BigReal.h"

class BigInt : public BigReal {
private:
  BigInt &operator=(const BigReal  &x); // not implemented
  BigInt &operator=(float           x); // not implemented
  BigInt &operator=(double          x); // not implemented
  BigInt &operator=(const Double80 &x); // not implemented

public:
  inline BigInt(DigitPool *digitPool = NULL) : BigReal(digitPool) {
  }

  // Declared explicit to avoid accidently use of operator/ on BigReals
  explicit BigInt(const BigReal &x, DigitPool *digitPool = NULL);

  BigInt(       const BigInt          &x, DigitPool *digitPool = NULL);
  inline BigInt(int                    x, DigitPool *digitPool = NULL) : BigReal(x, digitPool) {
  }
  inline BigInt(UINT                   x, DigitPool *digitPool = NULL) : BigReal(x, digitPool) {
  }
  inline BigInt(long                   x, DigitPool *digitPool = NULL) : BigReal(x, digitPool) {
  }
  inline BigInt(ULONG                  x, DigitPool *digitPool = NULL) : BigReal(x, digitPool) {
  }
  inline BigInt(INT64                  x, DigitPool *digitPool = NULL) : BigReal(x, digitPool) {
  }
  inline BigInt(UINT64                 x, DigitPool *digitPool = NULL) : BigReal(x, digitPool) {
  }
  inline BigInt(const _int128         &x, DigitPool *digitPool = NULL) : BigReal(x, digitPool) {
  }
  inline BigInt(const _uint128        &x, DigitPool *digitPool = NULL) : BigReal(x, digitPool) {
  }

  explicit BigInt(const String  &s, DigitPool *digitPool = NULL);
  explicit BigInt(const char    *s, DigitPool *digitPool = NULL);
  explicit BigInt(const wchar_t *s, DigitPool *digitPool = NULL);

  inline BigInt &operator=(const BigInt &x) {
    __super::operator=(x);
    return *this;
  }

  inline BigInt &operator=(int              n) {
    __super::operator=(n);
    return *this;
  }
  inline BigInt &operator=(UINT             n) {
    __super::operator=(n);
    return *this;
  }
  inline BigInt &operator=(long             n) {
    __super::operator=(n);
    return *this;
  }
  inline BigInt &operator=(ULONG            n) {
    __super::operator=(n);
    return *this;
  }
  inline BigInt &operator=(INT64            n) {
    __super::operator=(n);
    return *this;
  }
  inline BigInt &operator=(UINT64           n) {
    __super::operator=(n);
    return *this;
  }

  inline BigInt &operator=(const _int128   &n) {
    __super::operator=(n);
    return *this;
  }
  inline BigInt &operator=(const _uint128  &n) {
    __super::operator=(n);
    return *this;
  }

  friend inline void quotRemainder(const BigInt &x, const BigInt &y, BigInt *quotient, BigInt *remainder) {
#ifdef IS32BIT
    quotRemainder64((BigReal&)x, (BigReal&)y, quotient, remainder);
#else
    quotRemainder128((BigReal&)x, (BigReal&)y, quotient, remainder);
#endif
  }
  friend inline BigInt sum(const BigInt &x, const BigInt &y, DigitPool *digitPool = NULL) {
    return (BigInt&)sum((const BigReal&)x, (const BigReal&)y, (const BigReal&)BigReal::_0, digitPool);
  }
  friend inline BigInt dif(const BigInt &x, const BigInt &y, DigitPool *digitPool = NULL) {
    return (BigInt&)dif((const BigReal&)x, (const BigReal&)y, (const BigReal&)BigReal::_0, digitPool);
  }
  friend inline BigInt prod(const BigInt &x, const BigInt &y, DigitPool *digitPool = NULL) {
    return (BigInt&)prod((const BigReal&)x, (const BigReal&)y, (const BigReal&)BigReal::_0, digitPool);
  }
  inline BigInt operator+(const BigInt &y) const {
    return sum(*this, y, getDigitPool());
  }
  inline BigInt operator-(const BigInt &y) const {
    return dif(*this, y, getDigitPool());
  }
  inline BigInt operator*(const BigInt &y) const {
    return prod(*this, y, getDigitPool());
  }
  inline BigInt operator/(const BigInt &y) const {
    return quot(*this, y);
  }
  inline BigInt operator%(const BigInt &y) const {
    return rem(*this, y, getDigitPool());
  }

  inline BigInt &operator+=(const BigInt  &y) {
    return *this = *this + y;
  }
  inline BigInt &operator-=(const BigInt  &y) {
    return *this = *this - y;
  }
  inline BigInt &operator*=(const BigInt  &y) {
    return *this = *this * y;
  }
  inline BigInt &operator/=(const BigInt  &y) {
    return *this = *this / y;
  }
  inline BigInt &operator%=(const BigInt  &y) {
    return *this = *this % y;
  }
  inline BigInt operator-() const { // unary minus
    BigInt result(*this);
    result.clrInitDone().changeSign().setInitDone();
    return result;
  }
  inline BigInt &operator++() {                                              // prefix-form
    return (BigInt&)__super::operator++();
  }
  inline BigInt &operator--() {                                              // prefix-form
    return (BigInt&)__super::operator--();
  }
  inline BigInt operator++(int) {                                            // postfix-form
    const BigInt result(*this);
    __super::operator++();
    return result;
  }
  inline BigInt operator--(int) {                                            // postfix-form
    const BigInt result(*this);
    __super::operator--();
    return result;
  }
  // Fast version of *this *= 2
  inline BigInt &multiply2() {
    __super::multiply2();
    return *this;
  }
  // Fast version of *this /= 2
  BigInt &divide2();

  static const ConstBigInt _BINT_QNAN;  // non-signaling NaN (quiet NaN)
  static const ConstBigInt _BINT_PINF;  // +infinity;
  static const ConstBigInt _BINT_NINF;  // -infinity;

  // Checks that this is a consistent BigReal with all the various invariants satisfied.
  // Throws an excpeption if not with a descripion of what is wrong. For debugging
  virtual void assertIsValid(const TCHAR *file, int line, const TCHAR *name) const;
};

inline int sign(const BigInt &n) {
  return sign((BigReal&)n);
}

// pow(a,r) mod n
BigInt  powmod(const BigInt &a, const BigInt &r, const BigInt  &n, bool verbose = false, DigitPool *digitPool = NULL);

// Return uniform distributed random BigInt in [0..n-1], digits generated with rnd.
// If digitPool == NULL, use n.getDigitPool()
BigInt randBigInt(const BigInt &n, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator, DigitPool *digitPool = NULL);

// Return uniform distributed random BigInt in [from;to], digits generated with rnd.
// If digitPool == NULL, use from.getDigitPool()
BigInt randBigInt(const BigInt &from, const BigInt &to, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator, DigitPool *digitPool = NULL);

class ConstBigInt : public BigInt {
protected:
  bool allowConstDigitPool() const {
    return true;
  }

public:
  explicit ConstBigInt(const BigReal        &x) : BigInt(x, CONST_DIGITPOOL) {
  };
  inline ConstBigInt(int                     x) : BigInt(x, CONST_DIGITPOOL) {
  }
  inline ConstBigInt(UINT                    x) : BigInt(x, CONST_DIGITPOOL) {
  }
  inline ConstBigInt(long                    x) : BigInt(x, CONST_DIGITPOOL) {
  }
  inline ConstBigInt(ULONG                   x) : BigInt(x, CONST_DIGITPOOL) {
  }
  inline ConstBigInt(INT64                   x) : BigInt(x, CONST_DIGITPOOL) {
  }
  inline ConstBigInt(UINT64                  x) : BigInt(x, CONST_DIGITPOOL) {
  }
  inline ConstBigInt(const _int128          &x) : BigInt(x, CONST_DIGITPOOL) {
  }
  inline ConstBigInt(const _uint128         &x) : BigInt(x, CONST_DIGITPOOL) {
  }
  explicit ConstBigInt(const String         &s) : BigInt(s, CONST_DIGITPOOL) {
  }
  explicit ConstBigInt(const char           *s) : BigInt(s, CONST_DIGITPOOL) {
  }
  explicit ConstBigInt(const wchar_t        *s) : BigInt(s, CONST_DIGITPOOL) {
  }
};

inline bool isfinite(const BigInt &x) {
  return fpclassify(x) <= 0;
}
inline bool isinf(const BigInt &x) {
  return x._isinf();
}
inline bool isnan(const BigInt &x) {
  return x._isnan();
}
inline bool isnormal(const BigInt &x) {
  return x._isnormal();
}
inline bool isunordered(const BigInt &x, const BigInt &y) {
  return x._isnan() || y._isnan();
}

// save/load BigInt in binary format. Packer can be streamed to ByteOutputStream and read from ByteInputStream
Packer &operator<<(Packer &p, const BigInt &v);
// Call CHECKISMUTABLE
Packer &operator>>(Packer &p, BigInt &v);


std::istream     &operator>>(std::istream  &in ,       BigInt            &x);
std::ostream     &operator<<(std::ostream  &out, const BigInt            &x);
std::wistream    &operator>>(std::wistream &in,        BigInt            &x);
std::wostream    &operator<<(std::wostream &out, const BigInt            &x);

String      toString(const BigInt            &x, StreamSize width = 0, FormatFlags flags = 0, TCHAR separatorChar = 0);
BigInt      inputBigInt(DigitPool &digitPool, _In_z_ _Printf_format_string_ TCHAR const * const format, ...);
