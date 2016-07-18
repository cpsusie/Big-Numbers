#pragma once

#ifdef _M_X64

#include "Int128x64.h"

#else

#include "Int128x86.h"

#endif //  _M_X64

// 4 version of all 5 binary arithmetic operators 
// 3 binary logical operators
// and 6 compare-operators
//  signed   op signed
//  signed   op unsigned
//  unsigned op signed
//  unsigned op unsigned
//  For +,- and * the called function are the same for all 4 combinations.
//  For / and % however the signed function is used only for the "signed op signed" combination.
//  and unsigned version of the function is used in the other 3
// Vesion for build in integral types are then defined on top of these

// 2 more combination of operator+ (128-bit integers)
inline _int128 operator+(const _int128 &lft, const _uint128 &rhs) {
  _int128 result(lft);
  result += rhs;
  return result;
}
inline _uint128 operator+(const _uint128 &lft, const _int128 &rhs) {
  _uint128 result(lft);
  result += rhs;
  return result;
}

// 2 more combination of operator- (128-bit integers)
inline _int128 operator-(const _int128 &lft, const _uint128 &rhs) {
  _int128 result(lft);
  result -= rhs;
  return result;
}
inline _uint128 operator-(const _uint128 &lft, const _int128 &rhs) {
  _uint128 result(lft);
  result -= rhs;
  return result;
}

// 2 more combination of operator* (128-bit integers)
inline _int128 operator*(const _int128 &lft, const _uint128 &rhs) {
  _int128 result(lft);
  result *= rhs;
  return result;
}
inline _uint128 operator*(const _uint128 &lft, const _int128 &rhs) {
  _uint128 result(lft);
  result *= rhs;
  return result;
}

inline _uint128 operator/(const _uint128 &lft, const _int128 &rhs) {
  return lft / (const _uint128&)rhs;
}
inline _uint128 operator%(const _uint128 &lft, const _int128 &rhs) {
  return lft % (const _uint128&)rhs;
}


// HERTIL

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

inline _int128 operator&(const _int128 &lft, const _int128 &rhs) {
  return _int128(HI(lft) & HI(rhs), LO(lft) & LO(rhs));
}
inline _int128 operator&(const _int128 &lft, const _uint128 &rhs) {
  return _int128(HI(lft) & HI(rhs), LO(lft) & LO(rhs));
}
inline _uint128 operator&(const _uint128 &lft, const _int128 &rhs) {
  return _uint128(HI(lft) & HI(rhs), LO(lft) & LO(rhs));
}
inline _uint128 operator&(const _uint128 &lft, const _uint128 &rhs) {
  return _int128(HI(lft) & HI(rhs), LO(lft) & LO(rhs));
}
inline _int128 operator|(const _int128 &lft, const _int128 &rhs) {
  return _int128(HI(lft) | HI(rhs), LO(lft) | LO(rhs));
}
inline _int128 operator|(const _int128 &lft, const _uint128 &rhs) {
  return _int128(HI(lft) | HI(rhs), LO(lft) | LO(rhs));
}
inline _uint128 operator|(const _uint128 &lft, const _int128 &rhs) {
  return _uint128(HI(lft) | HI(rhs), LO(lft) | LO(rhs));
}
inline _uint128 operator|(const _uint128 &lft, const _uint128 &rhs) {
  return _uint128(HI(lft) | HI(rhs), LO(lft) | LO(rhs));
}
inline _int128 operator^(const _int128 &lft, const _int128 &rhs) {
  return _int128(HI(lft) ^ HI(rhs), LO(lft) ^ LO(rhs));
}
inline _int128 operator^(const _int128 &lft, const _uint128 &rhs) {
  return _int128(HI(lft) ^ HI(rhs), LO(lft) ^ LO(rhs));
}
inline _uint128 operator^(const _uint128 &lft, const _int128 &rhs) {
  return _uint128(HI(lft) ^ HI(rhs), LO(lft) ^ LO(rhs));
}
inline _uint128 operator^(const _uint128 &lft, const _uint128 &rhs) {
  return _uint128(HI(lft) ^ HI(rhs), LO(lft) ^ LO(rhs));
}
inline _int128 operator~(const _int128 &n) {
  return _int128(~HI(n), ~LO(n));
}
inline _uint128 operator~(const _uint128 &n) {
  return _uint128(~HI(n), ~LO(n));
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
  return lft & (_int128)rhs;
}
inline _uint128 operator&(const _uint128 &lft, unsigned __int64 rhs) {
  return lft & (_uint128)rhs;
}
inline _uint128 operator&(const _uint128 &lft, int rhs) {
  return lft & (_int128)rhs;
}
inline _uint128 operator&(const _uint128 &lft, unsigned int rhs) {
  return lft & (_uint128)rhs;
}
inline _uint128 operator&(const _uint128 &lft, short rhs) {
  return lft & (_int128)rhs;
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
  return lft | (_int128)rhs;
}
inline _uint128 operator|(const _uint128 &lft, unsigned __int64 rhs) {
  return lft | (_uint128)rhs;
}
inline _uint128 operator|(const _uint128 &lft, int rhs) {
  return lft | (_int128)rhs;
}
inline _uint128 operator|(const _uint128 &lft, unsigned int rhs) {
  return lft | (_uint128)rhs;
}
inline _uint128 operator|(const _uint128 &lft, short rhs) {
  return lft | (_int128)rhs;
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
  return lft ^ (_int128)rhs;
}
inline _uint128 operator^(const _uint128 &lft, unsigned __int64 rhs) {
  return lft ^ (_uint128)rhs;
}
inline _uint128 operator^(const _uint128 &lft, int rhs) {
  return lft ^ (_int128)rhs;
}
inline _uint128 operator^(const _uint128 &lft, unsigned int rhs) {
  return lft ^ (_uint128)rhs;
}
inline _uint128 operator^(const _uint128 &lft, short rhs) {
  return lft ^ (_int128)rhs;
}
inline _uint128 operator^(const _uint128 &lft, unsigned short rhs) {
  return lft ^ (_uint128)rhs;
}

inline bool operator==(const _int128 &lft, const _int128 &rhs) {
  return (LO(lft) == LO(rhs)) && (HI(lft) == HI(rhs));
}
inline bool operator==(const _int128 &lft, const _uint128 &rhs) {
  return (LO(lft) == LO(rhs)) && (HI(lft) == HI(rhs));
}
inline bool operator==(const _uint128 &lft, const _int128 &rhs) {
  return (LO(lft) == LO(rhs)) && (HI(lft) == HI(rhs));
}
inline bool operator==(const _uint128 &lft, const _uint128 &rhs) {
  return (LO(lft) == LO(rhs)) && (HI(lft) == HI(rhs));
}

// operator== for built in integral types as second argument
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

inline bool operator!=(const _int128 &lft, const _int128 &rhs) {
  return (LO(lft) != LO(rhs)) || (HI(lft) != HI(rhs));
}
inline bool operator!=(const _int128 &lft, const _uint128 &rhs) {
  return (LO(lft) != LO(rhs)) || (HI(lft) != HI(rhs));
}
inline bool operator!=(const _uint128 &lft, const _int128 &rhs) {
  return (LO(lft) != LO(rhs)) || (HI(lft) != HI(rhs));
}
inline bool operator!=(const _uint128 &lft, const _uint128 &rhs) {
  return (LO(lft) != LO(rhs)) || (HI(lft) != HI(rhs));
}

// operator!= for built in integral types as second argument
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
inline bool operator>(const _uint128 &lft, const _int128 &rhs) {
  return uint128cmp(&lft, &rhs) > 0;
}
inline bool operator>(const _uint128 &lft, const _uint128 &rhs) {
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
inline bool operator>=(const _uint128 &lft, const _int128 &rhs) {
  return uint128cmp(&lft, &rhs) >= 0;
}
inline bool operator>=(const _uint128 &lft, const _uint128 &rhs) {
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
inline bool operator<(const _uint128 &lft, const _int128 &rhs) {
  return uint128cmp(&lft, &rhs) < 0;
}
inline bool operator<(const _uint128 &lft, const _uint128 &rhs) {
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
inline bool operator<=(const _uint128 &lft, const _int128 &rhs) {
  return uint128cmp(&lft, &rhs) <= 0;
}
inline bool operator<=(const _uint128 &lft, const _uint128 &rhs) {
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
  return uint64Hash(LO(n) ^ HI(n));
}

inline unsigned long uint128Hash(const _uint128 &n) {
  return uint64Hash(LO(n) ^ HI(n));
}

inline int int128HashCmp(const _int128 &n1, const _int128 &n2) {
  return int128cmp(&n1, &n2);
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

