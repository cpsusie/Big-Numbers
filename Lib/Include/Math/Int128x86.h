#pragma once

#ifdef _M_X64

#error _int128 (x64-version) is available only in x86 architecture

#endif

#include "PragmaLib.h"

typedef struct {
  unsigned __int64 i[2];
} _S2;

typedef struct {
  unsigned int i[4];
} _S4;

typedef struct {
  unsigned short s[8];
} _S8;

typedef struct {
  unsigned char b[16];
} _S16;

#define HI(n) (n).s2.i[1]
#define LO(n) (n).s2.i[0]

void int128add( void *dst, const void *x);
void int128sub( void *dst, const void *x);
void int128mul( void *dst, const void *x);
void int128div( void *dst, const void *x);
void int128rem( void *dst, const void *x);
void int128neg( void *x);
void int128inc( void *x);
void int128dec( void *x);
void int128shr( void *x, int shft);
void int128shl( void *x, int shft);
int  int128cmp( const void *n1, const void *n2);
void uint128div(void *dst, const void *x);
void uint128rem(void *dst, const void *x);
void uint128shr(void *x, int shft);
int  uint128cmp(const void *n1, const void *n2);

class _int128 {
public:
  union {
    _S2  s2;
    _S4  s4;
    _S8  s8;
    _S16 s16;
  };

  // constructors
  inline _int128() {}
  inline _int128(const unsigned __int64 &n) {
    HI(*this) = 0;
    LO(*this) = n;
  }
  inline _int128(const __int64 &n) {
    HI(*this) = n < 0 ? -1 : 0;
    LO(*this) = n;
  }
  inline _int128(unsigned int n) {
    HI(*this) = 0;
    LO(*this) = n;
  }
  inline _int128(int n) {
    HI(*this) = n < 0 ? -1 : 0;
    LO(*this) = n;
  }
  inline _int128(unsigned short n) {
    HI(*this) = 0;
    LO(*this) = n;
  }
  inline _int128(short n) {
    HI(*this) = n < 0 ? -1 : 0;
    LO(*this) = n;
  }
  explicit inline _int128(const unsigned __int64 &hi, const unsigned __int64 &lo) {
    HI(*this) = hi;
    LO(*this) = lo;
  }
  explicit _int128(const char    *s);
  explicit _int128(const wchar_t *s);

  // type operators
  operator unsigned __int64() const {
    return LO(*this);
  }
  operator __int64() const {
    return LO(*this);
  }
  operator unsigned int() const {
    return (unsigned int)LO(*this);
  }
  operator int() const {
    return (int)LO(*this);
  }
  inline operator bool() const {
    return LO(*this) || HI(*this);
  }

  // arithmetic operators
  inline _int128 operator+(const _int128 &rhs) const {
    _int128 result(*this);
    int128add(&result, &rhs);
    return result;
  }
  inline _int128 operator-(const _int128 &rhs) const { // binary minus
    _int128 result(*this);
    int128sub(&result, &rhs);
    return result;
  }
  _int128 operator*(const _int128 &rhs) const {
    _int128 result(*this);
    int128mul(&result, &rhs);
    return result;
  }
  _int128 operator/(const _int128 &rhs) const {
    _int128 result(*this), tmp(rhs);
    int128div(&result, &tmp);
    return result;
  }
  _int128 operator%(const _int128 &rhs) const {
    _int128 result(*this), tmp(rhs);
    int128rem(&result, &tmp);
    return result;
  }
  _int128 operator-() const { // unary minus
    _int128 result(*this);
    int128neg(&result);
    return result;
  }

  // bitwise operators
  inline _int128 operator>>(const int shft) const {
    _int128 copy(*this);
    int128shr(&copy, shft);
    return copy;
  }
  inline _int128 operator<<(const int shft) const {
    _int128 copy(*this);
    int128shl(&copy, shft);
    return copy;
  }

  // assign operators
  inline _int128 &operator+=(const _int128 &rhs) {
    int128add(this, &rhs);
    return *this;
  }
  inline _int128 &operator-=(const _int128 &rhs) {
    int128sub(this, &rhs);
    return *this;
  }
  inline _int128 &operator*=(const _int128 &rhs) {
    int128mul(this, &rhs);
    return *this;
  }
  inline _int128 &operator/=(const _int128 &rhs) {
    _int128 tmp(rhs);
    int128div(this, &tmp);
    return *this;
  }
  inline _int128 &operator%=(const _int128 &rhs) {
    _int128 tmp(rhs);
    int128rem(this, &tmp);
    return *this;
  }

  inline _int128 &operator++() {   // prefix-form
    int128inc(this);
    return *this;
  }
  inline _int128 &operator--() {   // prefix-form
    int128dec(this);
    return *this;
  }

  inline _int128 operator++(int) { // postfix-form
    const _int128 result(*this);
    int128inc(this);
    return result;
  }
  inline _int128 operator--(int) { // postfix-form
    const _int128 result(*this);
    int128dec(this);
    return result;
  }

  inline _int128 &operator>>=(const int shft) {
    int128shr(this, shft);
    return *this;
  }
  inline _int128 &operator<<=(const int shft) {
    int128shl(this, shft);
    return *this;
  }
  inline _int128 &operator&=(const _int128 &rhs) {
    LO(*this) &= LO(rhs); HI(*this) &= HI(rhs); return *this;
  }
  inline _int128 &operator|=(const _int128 &rhs) {
    LO(*this) |= LO(rhs); HI(*this) |= HI(rhs); return *this;
  }
  inline _int128 &operator^=(const _int128 &rhs) {
    LO(*this) ^= LO(rhs); HI(*this) ^= HI(rhs); return *this;
  }
  bool isNegative() const {
    return ((int)s4.i[3] < 0);
  }
  bool isZero() const {
    return LO(*this) == 0 && HI(*this) == 0;
  }

  const char *parseDec(const char *str); // return pointer to char following the number, or NULL on error
  const char *parseHex(const char *str); // do
  const char *parseOct(const char *str); // do

  const wchar_t *parseDec(const wchar_t *str); // return pointer to char following the number
  const wchar_t *parseHex(const wchar_t *str); // do
  const wchar_t *parseOct(const wchar_t *str); // do
};

class _uint128 {
public:
  union {
    _S2  s2;
    _S4  s4;
    _S8  s8;
    _S16 s16;
  };

  // helpers for division
  void approxQuot(unsigned long y, int scale);
  unsigned int getFirstBitsAutoScale(int count, int &scale) const;
  unsigned int getFirstBitsNoScale(int count, int &expo) const;

  // constructors
  inline _uint128() {}
  inline _uint128(const _int128 &n) {
    HI(*this) = HI(n);
    LO(*this) = LO(n);
  }
  inline _uint128(const unsigned __int64 &n) {
    HI(*this) = 0;
    LO(*this) = n;
  }
  inline _uint128(const __int64 &n) {
    HI(*this) = n < 0 ? -1 : 0;
    LO(*this) = n;
  }
  inline _uint128(unsigned int n) {
    HI(*this) = 0;
    LO(*this) = n;
  }
  inline _uint128(int n) {
    HI(*this) = n < 0 ? -1 : 0;
    LO(*this) = n;
  }
  inline _uint128(unsigned short n) {
    HI(*this) = n < 0 ? -1 : 0;
    LO(*this) = n;
  }
  inline _uint128(short n) {
    HI(*this) = n < 0 ? -1 : 0;
    LO(*this) = n;
  }
  inline _uint128(const unsigned __int64 &hi, const unsigned __int64 &lo) {
    HI(*this) = hi;
    LO(*this) = lo;
  }
  explicit _uint128(const char    *s);
  explicit _uint128(const wchar_t *s);

  // type operators
  inline operator _int128() const {
    return *(_int128*)(void*)this;
  }
  inline operator unsigned __int64() const {
    return LO(*this);
  }
  inline operator __int64() const {
    return LO(*this);
  }
  inline operator unsigned int() const {
    return (unsigned int)LO(*this);
  }
  inline operator int() const {
    return (int)LO(*this);
  }
  inline operator bool() const {
    return LO(*this) || HI(*this);
  }

  // arithmetic operators
  inline _uint128 operator+(const _uint128 &rhs) const {
    _uint128 result(*this);
    int128add(&result, &rhs);
    return result;
  }
  _uint128 operator-(const _uint128 &rhs) const { // binary minus
    _uint128 result(*this);
    int128sub(&result, &rhs);
    return result;
  }
  inline _uint128 operator*(const _uint128 &rhs) const {
    _uint128 result(*this);
    int128mul(&result, &rhs);
    return result;
  }
  _uint128 operator/(const _uint128 &rhs) const {
    _uint128 result(*this);
    uint128div(&result, &rhs);
    return result;
  }
  _uint128 operator%(const _uint128 &rhs) const {
    _uint128 result(*this);
    uint128rem(&result, &rhs);
    return result;
  }

  inline _uint128 operator-() const {
    _uint128 result(*this);
    int128neg(&result);
    return result;
  }

  // bitwise operators
  inline _uint128 operator>>(const int shft) const {
    _uint128 copy(*this);
    uint128shr(&copy, shft);
    return copy;
  }
  inline _uint128 operator<<(const int shft) const {
    _uint128 copy(*this);
    int128shl(&copy, shft);
    return copy;
  }

  // assign operators
  inline _uint128 &operator+=(const _uint128 &rhs) {
    int128add(this, &rhs);
    return *this;
  }
  inline _uint128 &operator-=(const _uint128 &rhs) {
    int128sub(this, &rhs);
    return *this;
  }
  inline _uint128 &operator*=(const _uint128 &rhs) {
    int128mul(this, &rhs);
    return *this;
  }
  inline _uint128 &operator/=(const _uint128 &rhs) {
    uint128div(this, &rhs);
    return *this;
  }
  inline _uint128 &operator%=(const _uint128 &rhs) {
    uint128rem(this, &rhs);
    return *this;
  }

  inline _uint128 &operator++() {   // prefix-form
    int128inc(this);
    return *this;
  }
  inline _uint128 &operator--() {   // prefix-form
    int128dec(this);
    return *this;
  }

  inline _uint128 operator++(int) { // postfix-form
    const _uint128 result(*this);
    int128inc(this);
    return result;
  }
  inline _uint128 operator--(int) { // postfix-form
    const _uint128 result(*this);
    int128dec(this);
    return result;
  }
  inline _uint128 &operator>>=(const int shft) {
    uint128shr(this, shft);
    return *this;
  }
  inline _uint128 &operator<<=(const int shft) {
    int128shl(this, shft);
    return *this;
  }
  inline _uint128 &operator&=(const _uint128 &rhs) {
    LO(*this) &= LO(rhs); HI(*this) &= HI(rhs);
    return *this;
  }
  inline _uint128 &operator|=(const _uint128 &rhs) {
    LO(*this) |= LO(rhs); HI(*this) |= HI(rhs);
    return *this;
  }
  inline _uint128 &operator^=(const _uint128 &rhs) {
    LO(*this) ^= LO(rhs); HI(*this) ^= HI(rhs);
    return *this;
  }

  void quotRemainder(const _uint128 &n, _uint128 *quot, _uint128 *rem) const;
  bool isZero() const {
    return LO(*this) == 0 && HI(*this) == 0;
  }

  const char *parseDec(const char *str); // return pointer to char following the number, or NULL on error
  const char *parseHex(const char *str); // do
  const char *parseOct(const char *str); // do

  const wchar_t *parseDec(const wchar_t *str); // return pointer to char following the number
  const wchar_t *parseHex(const wchar_t *str); // do
  const wchar_t *parseOct(const wchar_t *str); // do
};
