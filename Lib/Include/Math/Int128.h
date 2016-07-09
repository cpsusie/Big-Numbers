#pragma once

#include "PragmaLib.h"

#ifndef _M_X64

#error Int128 is available only in x64 arhcitecture

#else

extern "C" {
  void int128add( void *dst, const void *x);
  void int128sub( void *dst, const void *x);
  void int128mul( void *dst, const void *x);
  void int128div( void *dst, void *x);
  void int128rem( void *dst, void *x);
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
};

class _uint128;

class _int128 {
public:
  unsigned __int64 lo;
  unsigned __int64 hi;

  inline _int128() {
  }
  inline _int128(unsigned __int64 n) : lo(n), hi(0) {
  }
  inline _int128(__int64 n) : lo(n), hi((n>=0)?0:-1) { // remember signextend hi if n < 0 (2-complement)
  }
  inline _int128(unsigned int n) : lo(n), hi(0) {
  }
  inline _int128(int n) : lo(n), hi((n>=0)?0:-1) {
  }
  inline _int128(unsigned short n) : lo(n), hi(0) {
  }
  inline _int128(short n) : lo(n), hi((n>=0)?0:-1) {
  }
  inline explicit _int128(const unsigned __int64 _hi, unsigned __int64 _lo) : hi(_hi), lo(_lo) {
  }
  explicit _int128(const char    *str);
  explicit _int128(const wchar_t *wstr);

  operator unsigned __int64() const {
    return lo;
  }
  operator __int64() const {
    return lo;
  }
  operator unsigned int() const {
    return (unsigned int)lo;
  }
  operator int() const {
    return (int)lo;
  }
  inline operator bool() const {
    return lo || hi;
  }

  // arithmetic operators
  inline _int128 operator+(const _int128 &rhs) const {
    _int128 result(*this);
    int128add(&result, &rhs);
    return result;
  }
  inline _int128 operator+(const _uint128 &rhs) const {
    _int128 result(*this);
    int128add(&result, &rhs);
    return result;
  }
  inline _int128 operator-(const _int128 &rhs) const {
    _int128 result(*this);
    int128sub(&result, &rhs);
    return result;
  }
  inline _int128 operator-(const _uint128 &rhs) const {
    _int128 result(*this);
    int128sub(&result, &rhs);
    return result;
  }
  inline _int128 operator*(const _int128 &rhs) const {
    _int128 result(*this);
    int128mul(&result, &rhs);
    return result;
  }
  inline _int128 operator*(const _uint128 &rhs) const {
    _int128 result(*this);
    int128mul(&result, &rhs);
    return result;
  }
  inline _int128 operator/(const _int128 &rhs) const {
    _int128 result(*this), tmp(rhs);
    int128div(&result, &tmp);
    return result;
  }
  inline _int128 operator%(const _int128 &rhs) const {
    _int128 result(*this), tmp(rhs);
    int128rem(&result, &tmp);
    return result;
  }
  inline _int128 operator-() const {
    _int128 result(*this);
    int128neg(&result);
    return result;
  }

  // bitwise operators
  inline _int128 operator>>(const int amount) const {
    _int128 copy(*this);
    int128shr(&copy, amount);
    return copy;
  }
  inline _int128 operator<<(const int amount) const {
    _int128 copy(*this);
    int128shl(&copy, amount);
    return copy;
  }
  inline _int128 operator&(const _int128 &rhs) const {
    return _int128(hi&rhs.hi, lo&rhs.lo);
  }
  inline _int128 operator|(const _int128 &rhs) const {
    return _int128(hi|rhs.hi, lo|rhs.lo);
  }
  inline _int128 operator^(const _int128 &rhs) const {
    return _int128(hi^rhs.hi, lo^rhs.lo);
  }
  inline _int128 operator~() const {
    return _int128(~hi, ~lo);
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

  inline _int128 &operator>>=(const int amount) {
    int128shr(this, amount);
    return *this;
  }
  inline _int128 &operator<<=(const int amount) {
    int128shl(this, amount);
    return *this;
  }
  inline _int128 &operator&=(const _int128 &rhs) {
    hi &= rhs.hi; lo &= rhs.lo;
    return *this;
  }
  inline _int128 &operator|=(const _int128 &rhs) {
    hi |= rhs.hi; lo |= rhs.lo;
    return *this;
  }
  inline _int128 &operator^=(const _int128 &rhs) {
    hi ^= rhs.hi; lo ^= rhs.lo;
    return *this;
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
  unsigned __int64 lo;
  unsigned __int64 hi;

  inline _uint128() {
  }
  inline _uint128(const _int128 &n) : lo(n.lo), hi(n.hi) {
  }
  inline _uint128(unsigned __int64 n) : lo(n), hi(0) {
  }
  inline _uint128(__int64 n) : lo(n), hi((n>=0)?0:-1) {
  }
  inline _uint128(unsigned int n) : lo(n), hi(0) {
  }
  inline _uint128(int n) : lo(n), hi(((n>=0)?0:-1)) {
  }
  inline _uint128(unsigned short n) : lo(n), hi(0) {
  }
  inline _uint128(short n) : lo(n), hi((n>=0)?0:-1) {
  }
  inline explicit _uint128(const unsigned __int64 _hi, unsigned __int64 _lo) : hi(_hi), lo(_lo) {
  }
  explicit _uint128(const char    *str);
  explicit _uint128(const wchar_t *wstr);

  inline operator _int128() const {
    return *(_int128*)(void*)this;
  }
  inline operator unsigned __int64() const {
    return lo;
  }
  inline operator __int64() const {
    return lo;
  }
  inline operator unsigned int() const {
    return (unsigned int)lo;
  }
  inline operator int() const {
    return (int)lo;
  }
  inline operator bool() const {
    return lo || hi;
  }

  // arithmetic operators
  inline _uint128 operator+(const _uint128 &rhs) const {
    _uint128 result(*this);
    int128add(&result, &rhs);
    return result;
  }
  inline _uint128 operator+(const _int128 &rhs) const {
    _uint128 result(*this);
    int128add(&result, &rhs);
    return result;
  }
  inline _uint128 operator-(const _uint128 &rhs) const {
    _uint128 result(*this);
    int128sub(&result, &rhs);
    return result;
  }
  inline _uint128 operator-(const _int128 &rhs) const {
    _uint128 result(*this);
    int128sub(&result, &rhs);
    return result;
  }
  inline _uint128 operator*(const _uint128 &rhs) const {
    _uint128 result(*this);
    int128mul(&result, &rhs);
    return result;
  }
  inline _uint128 operator*(const _int128 &rhs) const {
    _uint128 result(*this);
    int128mul(&result, &rhs);
    return result;
  }
  inline _uint128 operator/(const _uint128 &rhs) const {
    _uint128 result(*this);
    uint128div(&result, &rhs);
    return result;
  }
  inline _uint128 operator/(const _int128 &rhs) const {
    _uint128 result(*this);
    uint128div(&result, &rhs);
    return result;
  }
  inline _uint128 operator%(const _uint128 &rhs) const {
    _uint128 result(*this);
    uint128rem(&result, &rhs);
    return result;
  }
  inline _uint128 operator%(const _int128 &rhs) const {
    _uint128 result(*this);
    uint128rem(&result, &rhs);
    return result;
  }
  inline _uint128 operator-() {
    _uint128 result(*this);
    int128neg(&result);
    return result;
  }

  // bitwise operators
  inline _uint128 operator>>(const int amount) const {
    _uint128 copy(*this);
    uint128shr(&copy, amount);
    return copy;
  }
  inline _uint128 operator<<(const int amount) const {
    _uint128 copy(*this);
    int128shl(&copy, amount);
    return copy;
  }

  inline _uint128 operator&(const _uint128 &rhs) const {
    return _uint128(hi&rhs.hi, lo&rhs.lo);
  }
  inline _uint128 operator|(const _uint128 &rhs) const {
    return _uint128(hi|rhs.hi, lo|rhs.lo);
  }
  inline _uint128 operator^(const _uint128 &rhs) const {
    return _uint128(hi^rhs.hi, lo^rhs.lo);
  }
  inline _uint128 operator~() const {
    return _uint128(~hi, ~lo);
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
  inline _uint128 &operator>>=(const int amount) {
    uint128shr(this, amount);
    return *this;
  }
  inline _uint128 &operator<<=(const int amount) {
    int128shl(this, amount);
    return *this;
  }
  inline _uint128 &operator&=(const _uint128 &rhs) {
    hi &= rhs.hi; lo &= rhs.lo;
    return *this;
  }
  inline _uint128 &operator|=(const _uint128 &rhs) {
    hi |= rhs.hi; lo |= rhs.lo;
    return *this;
  }
  inline _uint128 &operator^(const _uint128 &rhs) {
    hi ^= rhs.hi; lo ^= rhs.lo;
    return *this;
  }

  const char *parseDec(const char *str); // return pointer to char following the number
  const char *parseHex(const char *str); // do
  const char *parseOct(const char *str); // do

  const wchar_t *parseDec(const wchar_t *str); // return pointer to char following the number
  const wchar_t *parseHex(const wchar_t *str); // do
  const wchar_t *parseOct(const wchar_t *str); // do
};

// 4 version of all 5 binary arithmetic operators and 6 compare-operators
//  signed   op signed
//  signed   op unsigned
//  unsigned op signed
//  unsigned op unsigned
//  For +,- and * the called function are the same for all 4 combinations.
//  For / and % however the signed function is used only for the "signed op signed" combination.
//  and unsigned version of the function is used in the other 3

// operator+ for built in integral types as second argument
inline _int128  operator+(const _int128  &lft, __int64 rhs) {
  return lft + (_int128)rhs;
}
inline _int128  operator+(const _int128  &lft, unsigned __int64 rhs) {
  return lft + (_uint128)rhs;
}
inline _int128  operator+(const _int128  &lft, int   rhs) {
  return lft + (_int128)rhs;
}
inline _int128  operator+(const _int128  &lft, unsigned   int   rhs) {
  return lft + (_uint128)rhs;
}
inline _int128  operator+(const _int128  &lft, short rhs) {
  return lft + (_int128)rhs;
}
inline _int128  operator+(const _int128  &lft, unsigned   short rhs) {
  return lft + (_uint128)rhs;
}
inline _uint128 operator+(const _uint128 &lft, __int64 rhs) {
  return lft + (_int128)rhs;
}
inline _uint128 operator+(const _uint128 &lft, unsigned __int64 rhs) {
  return lft + (_uint128)rhs;
}
inline _uint128 operator+(const _uint128 &lft, int   rhs) {
  return lft + (_int128)rhs;
}
inline _uint128 operator+(const _uint128 &lft, unsigned   int   rhs) {
  return lft + (_uint128)rhs;
}
inline _uint128 operator+(const _uint128 &lft, short rhs) {
  return lft + (_int128)rhs;
}
inline _uint128 operator+(const _uint128 &lft, unsigned   short rhs) {
  return lft + (_uint128)rhs;
}


// operator- for built in integral types as second argument
inline _int128  operator-(const _int128  &lft, __int64 rhs) {
  return lft - (_int128)rhs;
}
inline _int128  operator-(const _int128  &lft, unsigned __int64 rhs) {
  return lft - (_uint128)rhs;
}
inline _int128  operator-(const _int128  &lft, int rhs) {
  return lft - (_int128)rhs;
}
inline _int128  operator-(const _int128  &lft, unsigned int rhs) {
  return lft - (_uint128)rhs;
}
inline _int128  operator-(const _int128  &lft, short rhs) {
  return lft - (_int128)rhs;
}
inline _int128  operator-(const _int128  &lft, unsigned short rhs) {
  return lft - (_uint128)rhs;
}
inline _uint128 operator-(const _uint128 &lft, __int64 rhs) {
  return lft - (_int128)rhs;
}
inline _uint128 operator-(const _uint128 &lft, unsigned __int64 rhs) {
  return lft - (_uint128)rhs;
}
inline _uint128 operator-(const _uint128 &lft, int rhs) {
  return lft - (_int128)rhs;
}
inline _uint128 operator-(const _uint128 &lft, unsigned int rhs) {
  return lft - (_uint128)rhs;
}
inline _uint128 operator-(const _uint128 &lft, short rhs) {
  return lft - (_int128)rhs;
}
inline _uint128 operator-(const _uint128 &lft, unsigned short rhs) {
  return lft - (_uint128)rhs;
}


// operator* for built in integral types as second argument
inline _int128  operator*(const _int128  &lft, __int64 rhs) {
  return lft * (_int128)rhs;
}
inline _int128  operator*(const _int128  &lft, unsigned __int64 rhs) {
  return lft * (_uint128)rhs;
}
inline _int128  operator*(const _int128  &lft, int rhs) {
  return lft * (_int128)rhs;
}
inline _int128  operator*(const _int128  &lft, unsigned int rhs) {
  return lft * (_uint128)rhs;
}
inline _int128  operator*(const _int128  &lft, short rhs) {
  return lft * (_int128)rhs;
}
inline _int128  operator*(const _int128  &lft, unsigned short rhs) {
  return lft * (_uint128)rhs;
}
inline _uint128 operator*(const _uint128 &lft, __int64 rhs) {
  return lft * (_int128)rhs;
}
inline _uint128 operator*(const _uint128 &lft, unsigned __int64 rhs) {
  return lft * (_uint128)rhs;
}
inline _uint128 operator*(const _uint128 &lft, int rhs) {
  return lft * (_int128)rhs;
}
inline _uint128 operator*(const _uint128 &lft, unsigned int rhs) {
  return lft * (_uint128)rhs;
}
inline _uint128 operator*(const _uint128 &lft, short rhs) {
  return lft * (_int128)rhs;
}
inline _uint128 operator*(const _uint128 &lft, unsigned short rhs) {
  return lft * (_uint128)rhs;
}


// operator/ for built in integral types as second argument
inline _int128  operator/(const _int128  &lft, __int64 rhs) {
  return lft / (_int128)rhs;
}
inline _int128  operator/(const _int128  &lft, unsigned __int64 rhs) {
  return lft / (_int128)rhs;
}
inline _int128  operator/(const _int128  &lft, int rhs) {
  return lft / (_int128)rhs;
}
inline _int128  operator/(const _int128  &lft, unsigned int rhs) {
  return lft / (_int128)rhs;
}
inline _int128  operator/(const _int128  &lft, short rhs) {
  return lft / (_int128)rhs;
}
inline _int128  operator/(const _int128  &lft, unsigned short rhs) {
  return lft / (_int128)rhs;
}
inline _uint128 operator/(const _uint128 &lft, __int64 rhs) {
  return lft / (_int128)rhs;
}
inline _uint128 operator/(const _uint128 &lft, unsigned __int64 rhs) {
  return lft / (_uint128)rhs;
}
inline _uint128 operator/(const _uint128 &lft, int rhs) {
  return lft / (_int128)rhs;
}
inline _uint128 operator/(const _uint128 &lft, unsigned int rhs) {
  return lft / (_uint128)rhs;
}
inline _uint128 operator/(const _uint128 &lft, short rhs) {
  return lft / (_int128)rhs;
}
inline _uint128 operator/(const _uint128 &lft, unsigned short rhs) {
  return lft / (_uint128)rhs;
}


// operator% for built in integral types as second argument
inline _int128  operator%(const _int128  &lft, __int64 rhs) {
  return lft % (_int128)rhs;
}
inline _int128  operator%(const _int128  &lft, unsigned __int64 rhs) {
  return lft % (_int128)rhs;
}
inline _int128  operator%(const _int128  &lft, int rhs) {
  return lft % (_int128)rhs;
}
inline _int128  operator%(const _int128  &lft, unsigned int rhs) {
  return lft % (_int128)rhs;
}
inline _int128  operator%(const _int128  &lft, short rhs) {
  return lft % (_int128)rhs;
}
inline _int128  operator%(const _int128  &lft, unsigned short rhs) {
  return lft % (_int128)rhs;
}
inline _uint128 operator%(const _uint128 &lft, __int64 rhs) {
  return lft % (_int128)rhs;
}
inline _uint128 operator%(const _uint128 &lft, unsigned __int64 rhs) {
  return lft % (_uint128)rhs;
}
inline _uint128 operator%(const _uint128 &lft, int rhs) {
  return lft % (_int128)rhs;
}
inline _uint128 operator%(const _uint128 &lft, unsigned int rhs) {
  return lft % (_uint128)rhs;
}
inline _uint128 operator%(const _uint128 &lft, short rhs) {
  return lft % (_int128)rhs;
}
inline _uint128 operator%(const _uint128 &lft, unsigned short rhs) {
  return lft % (_uint128)rhs;
}


// operator& for built in integral types as second argument
inline _int128  operator&(const _int128  &lft, __int64 rhs) {
  return lft & (_int128)rhs;
}
inline _int128  operator&(const _int128  &lft, unsigned __int64 rhs) {
  return lft & (_int128)rhs;
}
inline _int128  operator&(const _int128  &lft, int rhs) {
  return lft & (_int128)rhs;
}
inline _int128  operator&(const _int128  &lft, unsigned int rhs) {
  return lft & (_int128)rhs;
}
inline _int128  operator&(const _int128  &lft, short rhs) {
  return lft & (_int128)rhs;
}
inline _int128  operator&(const _int128  &lft, unsigned short rhs) {
  return lft & (_int128)rhs;
}
inline _uint128 operator&(const _uint128 &lft, __int64 rhs) {
  return lft & (_uint128)rhs;
}
inline _uint128 operator&(const _uint128 &lft, unsigned __int64 rhs) {
  return lft & (_uint128)rhs;
}
inline _uint128 operator&(const _uint128 &lft, int rhs) {
  return lft & (_uint128)rhs;
}
inline _uint128 operator&(const _uint128 &lft, unsigned int rhs) {
  return lft & (_uint128)rhs;
}
inline _uint128 operator&(const _uint128 &lft, short rhs) {
  return lft & (_uint128)rhs;
}
inline _uint128 operator&(const _uint128 &lft, unsigned short rhs) {
  return lft & (_uint128)rhs;
}


// operator| for built in integral types as second argument
inline _int128  operator|(const _int128  &lft, __int64 rhs) {
  return lft | (_int128)rhs;
}
inline _int128  operator|(const _int128  &lft, unsigned __int64 rhs) {
  return lft | (_int128)rhs;
}
inline _int128  operator|(const _int128  &lft, int rhs) {
  return lft | (_int128)rhs;
}
inline _int128  operator|(const _int128  &lft, unsigned int rhs) {
  return lft | (_int128)rhs;
}
inline _int128  operator|(const _int128  &lft, short rhs) {
  return lft | (_int128)rhs;
}
inline _int128  operator|(const _int128  &lft, unsigned short rhs) {
  return lft | (_int128)rhs;
}
inline _uint128 operator|(const _uint128 &lft, __int64 rhs) {
  return lft | (_uint128)rhs;
}
inline _uint128 operator|(const _uint128 &lft, unsigned __int64 rhs) {
  return lft | (_uint128)rhs;
}
inline _uint128 operator|(const _uint128 &lft, int rhs) {
  return lft | (_uint128)rhs;
}
inline _uint128 operator|(const _uint128 &lft, unsigned int rhs) {
  return lft | (_uint128)rhs;
}
inline _uint128 operator|(const _uint128 &lft, short rhs) {
  return lft | (_uint128)rhs;
}
inline _uint128 operator|(const _uint128 &lft, unsigned short rhs) {
  return lft | (_uint128)rhs;
}


// operator^ for built in integral types as second argument
inline _int128  operator^(const _int128  &lft, __int64 rhs) {
  return lft ^ (_int128)rhs;
}
inline _int128  operator^(const _int128  &lft, unsigned __int64 rhs) {
  return lft ^ (_int128)rhs;
}
inline _int128  operator^(const _int128  &lft, int rhs) {
  return lft ^ (_int128)rhs;
}
inline _int128  operator^(const _int128  &lft, unsigned int rhs) {
  return lft ^ (_int128)rhs;
}
inline _int128  operator^(const _int128  &lft, short rhs) {
  return lft ^ (_int128)rhs;
}
inline _int128  operator^(const _int128  &lft, unsigned short rhs) {
  return lft ^ (_int128)rhs;
}
inline _uint128 operator^(const _uint128 &lft, __int64 rhs) {
  return lft ^ (_uint128)rhs;
}
inline _uint128 operator^(const _uint128 &lft, unsigned __int64 rhs) {
  return lft ^ (_uint128)rhs;
}
inline _uint128 operator^(const _uint128 &lft, int rhs) {
  return lft ^ (_uint128)rhs;
}
inline _uint128 operator^(const _uint128 &lft, unsigned int rhs) {
  return lft ^ (_uint128)rhs;
}
inline _uint128 operator^(const _uint128 &lft, short rhs) {
  return lft ^ (_uint128)rhs;
}
inline _uint128 operator^(const _uint128 &lft, unsigned short rhs) {
  return lft ^ (_uint128)rhs;
}


// operator== for built in integral types as second argument
inline bool operator==(const _int128 &lft, const _int128 &rhs) {
  return (lft.lo == rhs.lo) && (lft.hi == rhs.hi);
}
inline bool operator==(const _int128 &lft, const _uint128 &rhs) {
  return (lft.lo == rhs.lo) && (lft.hi == rhs.hi);
}
inline bool operator==(const _int128 &lft, __int64 rhs) {
  return lft == _int128(rhs);
}
inline bool operator==(const _int128 &lft, unsigned __int64 rhs) {
  return lft == _int128(rhs);
}
inline bool operator==(const _int128 &lft, int rhs) {
  return lft == _int128(rhs);
}
inline bool operator==(const _int128 &lft, unsigned int rhs) {
  return lft == _int128(rhs);
}
inline bool operator==(const _int128 &lft, short rhs) {
  return lft == _int128(rhs);
}
inline bool operator==(const _int128 &lft, unsigned short rhs) {
  return lft == _int128(rhs);
}

inline bool operator==(const _uint128 &lft, const _int128 &rhs) {
  return (lft.lo == rhs.lo) && (lft.hi == rhs.hi);
}
inline bool operator==(const _uint128 &lft, const _uint128 &rhs) {
  return (lft.lo == rhs.lo) && (lft.hi == rhs.hi);
}
inline bool operator==(const _uint128 &lft, __int64 rhs) {
  return lft == _int128(rhs);
}
inline bool operator==(const _uint128 &lft, unsigned __int64 rhs) {
  return lft == _uint128(rhs);
}
inline bool operator==(const _uint128 &lft, int rhs) {
  return lft == _int128(rhs);
}
inline bool operator==(const _uint128 &lft, unsigned int rhs) {
  return lft == _uint128(rhs);
}
inline bool operator==(const _uint128 &lft, short rhs) {
  return lft == _int128(rhs);
}
inline bool operator==(const _uint128 &lft, unsigned short rhs) {
  return lft == _uint128(rhs);
}


// operator!= for built in integral types as second argument
inline bool operator!=(const _int128 &lft, const _int128 &rhs) {
  return (lft.lo != rhs.lo) || (lft.hi != rhs.hi);
}
inline bool operator!=(const _int128 &lft, const _uint128 &rhs) {
  return (lft.lo != rhs.lo) || (lft.hi != rhs.hi);
}
inline bool operator!=(const _int128 &lft, __int64 rhs) {
  return lft != _int128(rhs);
}
inline bool operator!=(const _int128 &lft, unsigned __int64 rhs) {
  return lft != _int128(rhs);
}
inline bool operator!=(const _int128 &lft, int rhs) {
  return lft != _int128(rhs);
}
inline bool operator!=(const _int128 &lft, unsigned int rhs) {
  return lft != _int128(rhs);
}
inline bool operator!=(const _int128 &lft, short rhs) {
  return lft != _int128(rhs);
}
inline bool operator!=(const _int128 &lft, unsigned short rhs) {
  return lft != _int128(rhs);
}
inline bool operator!=(const _uint128 &lft, const _int128 &rhs) {
  return (lft.lo != rhs.lo) || (lft.hi != rhs.hi);
}
inline bool operator!=(const _uint128 &lft, const _uint128 &rhs) {
  return (lft.lo != rhs.lo) || (lft.hi != rhs.hi);
}
inline bool operator!=(const _uint128 &lft, __int64 rhs) {
  return lft != _int128(rhs);
}
inline bool operator!=(const _uint128 &lft, unsigned __int64 rhs) {
  return lft != _uint128(rhs);
}
inline bool operator!=(const _uint128 &lft, int rhs) {
  return lft != _int128(rhs);
}
inline bool operator!=(const _uint128 &lft, unsigned int rhs) {
  return lft != _uint128(rhs);
}
inline bool operator!=(const _uint128 &lft, short rhs) {
  return lft != _int128(rhs);
}
inline bool operator!=(const _uint128 &lft, unsigned short rhs) {
  return lft != _uint128(rhs);
}

// operator> for built in integral types as second argument
inline bool operator>(const _int128 &lft, const _int128 &rhs) {
  return int128cmp(&lft, &rhs) > 0;
}
inline bool operator>(const _int128 &lft, const _uint128 &rhs) {
  return uint128cmp(&lft, &rhs) > 0;
}
inline bool operator>(const _int128 &lft, __int64 rhs) {
  return lft > _int128(rhs);
}
inline bool operator>(const _int128 &lft, unsigned __int64 rhs) {
  return lft > _uint128(rhs);
}
inline bool operator>(const _int128 &lft, int rhs) {
  return lft > _int128(rhs);
}
inline bool operator>(const _int128 &lft, unsigned int rhs) {
  return lft > _uint128(rhs);
}
inline bool operator>(const _int128 &lft, short rhs) {
  return lft > _int128(rhs);
}
inline bool operator>(const _int128 &lft, unsigned short rhs) {
  return lft > _uint128(rhs);
}
inline bool operator>(const _uint128 &lft, const _int128 &rhs) {
  return uint128cmp(&lft, &rhs) > 0;
}
inline bool operator>(const _uint128 &lft, const _uint128 &rhs) {
  return uint128cmp(&lft, &rhs) > 0;
}
inline bool operator>(const _uint128 &lft, __int64 rhs) {
  return lft > _int128(rhs);
}
inline bool operator>(const _uint128 &lft, unsigned __int64 rhs) {
  return lft > _uint128(rhs);
}
inline bool operator>(const _uint128 &lft, int rhs) {
  return lft > _int128(rhs);
}
inline bool operator>(const _uint128 &lft, unsigned int rhs) {
  return lft > _uint128(rhs);
}
inline bool operator>(const _uint128 &lft, short rhs) {
  return lft > _int128(rhs);
}
inline bool operator>(const _uint128 &lft, unsigned short rhs) {
  return lft > _uint128(rhs);
}


// operator>= for built in integral types as second argument
inline bool operator>=(const _int128 &lft, const _int128 &rhs) {
  return int128cmp(&lft, &rhs) >= 0;
}
inline bool operator>=(const _int128 &lft, const _uint128 &rhs) {
  return uint128cmp(&lft, &rhs) >= 0;
}
inline bool operator>=(const _int128 &lft, __int64 rhs) {
  return lft >= _int128(rhs);
}
inline bool operator>=(const _int128 &lft, unsigned __int64 rhs) {
  return lft >= _uint128(rhs);
}
inline bool operator>=(const _int128 &lft, int rhs) {
  return lft >= _int128(rhs);
}
inline bool operator>=(const _int128 &lft, unsigned int rhs) {
  return lft >= _uint128(rhs);
}
inline bool operator>=(const _int128 &lft, short rhs) {
  return lft >= _int128(rhs);
}
inline bool operator>=(const _int128 &lft, unsigned short rhs) {
  return lft >= _uint128(rhs);
}
inline bool operator>=(const _uint128 &lft, const _int128 &rhs) {
  return uint128cmp(&lft, &rhs) >= 0;
}
inline bool operator>=(const _uint128 &lft, const _uint128 &rhs) {
  return uint128cmp(&lft, &rhs) >= 0;
}
inline bool operator>=(const _uint128 &lft, __int64 rhs) {
  return lft >= _int128(rhs);
}
inline bool operator>=(const _uint128 &lft, unsigned __int64 rhs) {
  return lft >= _uint128(rhs);
}
inline bool operator>=(const _uint128 &lft, int rhs) {
  return lft >= _int128(rhs);
}
inline bool operator>=(const _uint128 &lft, unsigned int rhs) {
  return lft >= _uint128(rhs);
}
inline bool operator>=(const _uint128 &lft, short rhs) {
  return lft >= _int128(rhs);
}
inline bool operator>=(const _uint128 &lft, unsigned short rhs) {
  return lft >= _uint128(rhs);
}


// operator< for built in integral types as second argument
inline bool operator<(const _int128 &lft, const _int128 &rhs) {
  return int128cmp(&lft, &rhs) < 0;
}
inline bool operator<(const _int128 &lft, const _uint128 &rhs) {
  return uint128cmp(&lft, &rhs) < 0;
}
inline bool operator<(const _int128 &lft, __int64 rhs) {
  return lft < _int128(rhs);
}
inline bool operator<(const _int128 &lft, unsigned __int64 rhs) {
  return lft < _uint128(rhs);
}
inline bool operator<(const _int128 &lft, int rhs) {
  return lft < _int128(rhs);
}
inline bool operator<(const _int128 &lft, unsigned int rhs) {
  return lft < _uint128(rhs);
}
inline bool operator<(const _int128 &lft, short rhs) {
  return lft < _int128(rhs);
}
inline bool operator<(const _int128 &lft, unsigned short rhs) {
  return lft < _uint128(rhs);
}
inline bool operator<(const _uint128 &lft, const _int128 &rhs) {
  return uint128cmp(&lft, &rhs) < 0;
}
inline bool operator<(const _uint128 &lft, const _uint128 &rhs) {
  return uint128cmp(&lft, &rhs) < 0;
}
inline bool operator<(const _uint128 &lft, __int64 rhs) {
  return lft < _int128(rhs);
}
inline bool operator<(const _uint128 &lft, unsigned __int64 rhs) {
  return lft < _uint128(rhs);
}
inline bool operator<(const _uint128 &lft, int rhs) {
  return lft < _int128(rhs);
}
inline bool operator<(const _uint128 &lft, unsigned int rhs) {
  return lft < _uint128(rhs);
}
inline bool operator<(const _uint128 &lft, short rhs) {
  return lft < _int128(rhs);
}
inline bool operator<(const _uint128 &lft, unsigned short rhs) {
  return lft < _uint128(rhs);
}


// operator<= for built in integral types as second argument
inline bool operator<=(const _int128 &lft, const _int128 &rhs) {
  return int128cmp(&lft, &rhs) <= 0;
}
inline bool operator<=(const _int128 &lft, const _uint128 &rhs) {
  return uint128cmp(&lft, &rhs) <= 0;
}
inline bool operator<=(const _int128 &lft, __int64 rhs) {
  return lft <= _int128(rhs);
}
inline bool operator<=(const _int128 &lft, unsigned __int64 rhs) {
  return lft <= _uint128(rhs);
}
inline bool operator<=(const _int128 &lft, int rhs) {
  return lft <= _int128(rhs);
}
inline bool operator<=(const _int128 &lft, unsigned int rhs) {
  return lft <= _uint128(rhs);
}
inline bool operator<=(const _int128 &lft, short rhs) {
  return lft <= _int128(rhs);
}
inline bool operator<=(const _int128 &lft, unsigned short rhs) {
  return lft <= _uint128(rhs);
}
inline bool operator<=(const _uint128 &lft, const _int128 &rhs) {
  return uint128cmp(&lft, &rhs) <= 0;
}
inline bool operator<=(const _uint128 &lft, const _uint128 &rhs) {
  return uint128cmp(&lft, &rhs) <= 0;
}
inline bool operator<=(const _uint128 &lft, __int64 rhs) {
  return lft <= _int128(rhs);
}
inline bool operator<=(const _uint128 &lft, unsigned __int64 rhs) {
  return lft <= _uint128(rhs);
}
inline bool operator<=(const _uint128 &lft, int rhs) {
  return lft <= _int128(rhs);
}
inline bool operator<=(const _uint128 &lft, unsigned int rhs) {
  return lft <= _uint128(rhs);
}
inline bool operator<=(const _uint128 &lft, short rhs) {
  return lft <= _int128(rhs);
}
inline bool operator<=(const _uint128 &lft, unsigned short rhs) {
  return lft <= _uint128(rhs);
}


char    * _i128toa( _int128  value, char    *str, int radix);
char    * _ui128toa(_uint128 value, char    *str, int radix);
wchar_t * _i128tow( _int128  value, wchar_t *str, int radix);
wchar_t * _ui128tow(_uint128 value, wchar_t *str, int radix);


#ifdef _UNICODE
#define _i128tot   _i128tow
#define _ui128tot  _ui128tow
#else
#define _i128tot   _i128toa
#define _ui128tot  _ui128toa
#endif // _UNICODE

inline char radixLetter(unsigned int c) {
  return (c < 10) ? ('0' + c) : ('a' + (c-10));
}

inline wchar_t wradixLetter(unsigned int c) {
  return (c < 10) ? ('0' + c) : ('a' + (c-10));
}

inline bool iswodigit(wchar_t ch) {
  return ('0' <= ch) && (ch < '8');
}

unsigned int convertNumberChar(wchar_t digit);

extern const _int128  _I128_MIN, _I128_MAX;
extern const _uint128 _UI128_MAX;

inline unsigned long int128Hash(const _int128 &n) {
  return uint64Hash(n.lo ^ n.hi);
}

inline int int128HashCmp(const _int128 &n1, const _int128 &n2) {
  return int128cmp(&n1, &n2);
}

inline unsigned long uint128Hash(const _uint128 &n) {
  return uint64Hash(n.lo ^ n.hi);
}

inline int uint128HashCmp(const _uint128 &n1, const _uint128 &n2) {
  return uint128cmp(&n1, &n2);
}

std::istream  &operator>>(std::istream  &s,       _int128  &n);
std::ostream  &operator<<(std::ostream  &s, const _int128  &n);
std::istream  &operator>>(std::istream  &s,       _uint128 &n);
std::ostream  &operator<<(std::ostream  &s, const _uint128 &n);

std::wistream &operator>>(std::wistream &s,       _int128  &n);
std::wostream &operator<<(std::wostream &s, const _int128  &n);
std::wistream &operator>>(std::wistream &s,       _uint128 &n);
std::wostream &operator<<(std::wostream &s, const _uint128 &n);

#endif // _M_X64
