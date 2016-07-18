#pragma once

#ifndef _M_X64

#error _int128 (x64-version) is available only in x64 architecture

#else

#include "PragmaLib.h"

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

#define HI(n) (n).hi
#define LO(n) (n).lo

class _int128 {
public:
  unsigned __int64 lo;
  unsigned __int64 hi;

  // constructors
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

  // type operators
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
  inline _int128 operator-(const _int128 &rhs) const {
    _int128 result(*this);
    int128sub(&result, &rhs);
    return result;
  }
  inline _int128 operator*(const _int128 &rhs) const {
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

  // constructors
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

  // type operators
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
  inline _uint128 operator-(const _uint128 &rhs) const {
    _uint128 result(*this);
    int128sub(&result, &rhs);
    return result;
  }
  inline _uint128 operator*(const _uint128 &rhs) const {
    _uint128 result(*this);
    int128mul(&result, &rhs);
    return result;
  }
  inline _uint128 operator/(const _uint128 &rhs) const {
    _uint128 result(*this);
    uint128div(&result, &rhs);
    return result;
  }
  inline _uint128 operator%(const _uint128 &rhs) const {
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

#endif // _M_X64
