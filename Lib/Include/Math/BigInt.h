#pragma once

#include "BigReal.h"

class BigInt : public BigReal {
public:
  BigInt(DigitPool *digitPool = NULL) : BigReal(digitPool) {
  }

  // Declared explicit to avoid accidently use of operator/ on BigReals
  explicit BigInt(const BigReal &x, DigitPool *digitPool = NULL);

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
  BigInt &operator=(const BigReal &x);
  BigInt &operator/=(const BigInt  &x);
};

BigInt operator+(const BigInt &x, const BigInt &y);
BigInt operator-(const BigInt &x, const BigInt &y);
BigInt operator*(const BigInt &x, const BigInt &y);

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
