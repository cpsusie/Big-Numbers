#pragma once

#include <Windows.h>
#include <MathUtil.h>

// very common used hash- and comparefunctions
inline ULONG shortHash(const short &n) {
  return n;
}

inline int shortHashCmp(const short &n1, const short &n2) {
  return n1 - n2;
}

inline ULONG ushortHash(const USHORT &n) {
  return n;
}

inline int ushortHashCmp(const USHORT &n1, const USHORT &n2) {
  return (n1 > n2) ? 1 : (n1 < n2) ? -1 : 0;
}

inline ULONG intHash(const int &n) {
  return n;
}

inline int intHashCmp(const int &n1, const int &n2) {
  return n1 - n2;
}

inline ULONG uintHash(const UINT &n) {
  return n;
}

inline int  uintHashCmp(const UINT &n1, const UINT &n2) {
  return (n1 > n2) ? 1 : (n1 < n2) ? -1 : 0;
}

inline ULONG longHash(const long &n) {
  return n;
}

inline int longHashCmp(const long &n1, const long &n2) {
  return n1 - n2;
}

inline ULONG ulongHash(const ULONG &n) {
  return n;
}

inline int ulongHashCmp(const ULONG &n1, const ULONG &n2) {
  return (n1 > n2) ? 1 : (n1 < n2) ? -1 : 0;
}

inline ULONG int64Hash(const INT64 &n) {
  return (((ULONG*)&n)[0]) ^ (((ULONG*)&n)[1]);
}

inline int int64HashCmp(const INT64 &n1, const INT64 &n2) {
  return (n1 > n2) ? 1 : (n1 < n2) ? -1 : 0;
}

inline ULONG uint64Hash(const UINT64 &n) {
  return (((ULONG*)&n)[0]) ^ (((ULONG*)&n)[1]);
}

inline int uint64HashCmp(const UINT64 &n1, const UINT64 &n2) {
  return (n1 > n2) ? 1 : (n1 < n2) ? -1 : 0;
}

#ifdef IS64BIT
#define sizetHash    uint64Hash
#define sizetHashCmp uint64HashCmp
#else
#define sizetHash    uintHash
#define sizetHashCmp uintHashCmp
#endif

inline ULONG floatHash(const float &f) {
  return (ULONG)f;
}

inline int floatHashCmp(const float &f1, const float &f2) {
  return sign(f1 - f2);
}

inline ULONG doubleHash(const double &d) {
  const ULONG *p = (ULONG*)(&d);
  return p[0] ^ p[1];
}

inline int doubleHashCmp(const double &d1, const double &d2) {
  return sign(d1 - d2);
}

template<class T> ULONG pointerHash(const T * const &p) {
  return sizetHash((size_t)p);
}

template<class T> int pointerHashCmp(const T * const &p1, const T * const &p2) {
  return sizetHashCmp((size_t)p1, size_t(p2));
}

inline int boolCmp(bool b1, bool b2) {
  return ordinal(b1) - ordinal(b2);
}
