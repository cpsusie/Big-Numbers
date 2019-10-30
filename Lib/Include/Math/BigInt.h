#pragma once

#include "BigReal.h"

class BigInt : public BigReal {
private:
  BigInt &operator=(const BigReal  &x); // not implemented
  BigInt &operator=(float           x); // not implemented
  BigInt &operator=(double          x); // not implemented
  BigInt &operator=(const Double80 &x); // not implemented

public:
  BigInt(DigitPool *digitPool = NULL) : BigReal(digitPool) {
  }

  // Declared explicit to avoid accidently use of operator/ on BigReals
  explicit BigInt(const BigReal &x, DigitPool *digitPool = NULL);

  BigInt(         const BigInt &x, DigitPool *digitPool = NULL);
  BigInt(int                    x, DigitPool *digitPool = NULL) : BigReal(x, digitPool) {
  }
  BigInt(UINT                   x, DigitPool *digitPool = NULL) : BigReal(x, digitPool) {
  }
  BigInt(long                   x, DigitPool *digitPool = NULL) : BigReal(x, digitPool) {
  }
  BigInt(ULONG                  x, DigitPool *digitPool = NULL) : BigReal(x, digitPool) {
  }
  BigInt(INT64                  x, DigitPool *digitPool = NULL) : BigReal(x, digitPool) {
  }
  BigInt(UINT64                 x, DigitPool *digitPool = NULL) : BigReal(x, digitPool) {
  }
  BigInt(const _int128         &x, DigitPool *digitPool = NULL) : BigReal(x, digitPool) {
  }
  BigInt(const _uint128        &x, DigitPool *digitPool = NULL) : BigReal(x, digitPool) {
  }

  explicit BigInt(const String  &s, DigitPool *digitPool = NULL);
  explicit BigInt(const char    *s, DigitPool *digitPool = NULL);
  explicit BigInt(const wchar_t *s, DigitPool *digitPool = NULL);

  BigInt &operator=(const BigInt &x) {
    __super::operator=(x);
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
  friend BigInt rem(const BigInt  &x, const BigInt &y, DigitPool *digitPool = NULL);

  inline BigInt operator+(const BigInt &y) const {
    return sum(*this, y);
  }
  inline BigInt operator-(const BigInt &y) const {
    return dif(*this, y);
  }
  inline BigInt operator*(const BigInt &y) const {
    return prod(*this, y);
  }
  inline BigInt operator/(const BigInt &y) const {
    return quot(*this, y);
  }
  inline BigInt operator%(const BigInt &y) const {
    return rem(*this, y);
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
    result.changeSign();
    return result;
  }
  inline BigInt &operator++() {                                              // prefix-form
    return (BigInt&)__super::operator++();
  }
  inline BigInt &operator--() {                                              // prefix-form
    return (BigInt&)__super::operator--();
  }
  inline BigInt operator++(int) {                                            // postfix-form
    BigInt result(*this);
    __super::operator++();
    return result;
  }
  inline BigInt operator--(int) {                                            // postfix-form
    BigInt result(*this);
    __super::operator--();
    return result;
  }
};

// Return uniform distributed random BigInt in [0..e(1,maxDigits)-1], digits generated with rnd.
// If digitPool == NULL, use DEFAULT_DIGITPOOL
// ex:maxDigits = 3:returned values in interval [0..999]
BigInt randBigInt(size_t maxDigits, RandomGenerator *rnd = _standardRandomGenerator, DigitPool *digitPool = NULL);

// Return uniform distributed random BigInt in [0..n-1], digits generated with rnd.
// If digitPool == NULL, use n.getDigitPool()
BigInt randBigInt(const BigInt &n, RandomGenerator *rnd = _standardRandomGenerator, DigitPool *digitPool = NULL);

// Return uniform distributed random BigInt in [from;to], digits generated with rnd.
// If digitPool == NULL, use from.getDigitPool()
BigInt randBigInt(const BigInt &from, const BigInt &to, RandomGenerator *rnd = _standardRandomGenerator, DigitPool *digitPool = NULL);

#define REQUESTCONSTPOOL ConstDigitPool::requestInstance()
#define RELEASECONSTPOOL ConstDigitPool::releaseInstance()

class ConstBigInt : public BigInt {
public:
  explicit ConstBigInt(const BigReal &x) : BigInt(x, REQUESTCONSTPOOL) {
    RELEASECONSTPOOL;
  };
  ConstBigInt(int                     x) : BigInt(x, REQUESTCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  ConstBigInt(UINT                    x) : BigInt(x, REQUESTCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  ConstBigInt(long                    x) : BigInt(x, REQUESTCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  ConstBigInt(ULONG                   x) : BigInt(x, REQUESTCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  ConstBigInt(INT64                   x) : BigInt(x, REQUESTCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  ConstBigInt(UINT64                  x) : BigInt(x, REQUESTCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  ConstBigInt(const _int128          &x) : BigInt(x, REQUESTCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  ConstBigInt(const _uint128         &x) : BigInt(x, REQUESTCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  explicit ConstBigInt(const String  &s) : BigInt(s, REQUESTCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  explicit ConstBigInt(const char    *s) : BigInt(s, REQUESTCONSTPOOL) {
    RELEASECONSTPOOL;
  }
  explicit ConstBigInt(const wchar_t *s) : BigInt(s, REQUESTCONSTPOOL) {
    RELEASECONSTPOOL;
  }

  ~ConstBigInt() {
    REQUESTCONSTPOOL;
    releaseDigits();
    RELEASECONSTPOOL;
  }
};

#undef REQUESTCONSTPOOL
#undef RELEASECONSTPOOL

std::istream     &operator>>(std::istream  &in ,       BigInt            &x);
std::ostream     &operator<<(std::ostream  &out, const BigInt            &x);
std::wistream    &operator>>(std::wistream &in,        BigInt            &x);
std::wostream    &operator<<(std::wostream &out, const BigInt            &x);

String      toString(const BigInt            &x, StreamSize width = 0, FormatFlags flags = 0, TCHAR separatorChar = 0);
BigInt      inputBigInt(DigitPool &digitPool, _In_z_ _Printf_format_string_ TCHAR const * const format, ...);
