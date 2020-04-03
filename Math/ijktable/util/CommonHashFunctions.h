#pragma once

#include "util.h"

// very common used hash- and comparefunctions
inline ulong shortHash(const short &n) {
  return n;
}

inline int shortHashCmp(const short &n1, const short &n2) {
  return n1 - n2;
}

inline ulong ushortHash(const ushort &n) {
  return n;
}

inline int ushortHashCmp(const ushort &n1, const ushort &n2) {
  return (n1 > n2) ? 1 : (n1 < n2) ? -1 : 0;
}

inline ulong intHash(const int &n) {
  return n;
}

inline int intHashCmp(const int &n1, const int &n2) {
  return n1 - n2;
}

inline ulong uintHash(const uint &n) {
  return n;
}

inline int  uintHashCmp(const uint &n1, const uint &n2) {
  return (n1 > n2) ? 1 : (n1 < n2) ? -1 : 0;
}

inline ulong longHash(const long &n) {
  return n;
}

inline int longHashCmp(const long &n1, const long &n2) {
  return n1 - n2;
}

inline ulong ulongHash(const ulong &n) {
  return n;
}

inline int ulongHashCmp(const ulong &n1, const ulong &n2) {
  return (n1 > n2) ? 1 : (n1 < n2) ? -1 : 0;
}

inline ulong int64Hash(const int64 &n) {
  return (((ulong*)&n)[0]) ^ (((ulong*)&n)[1]);
}

inline int int64HashCmp(const int64 &n1, const int64 &n2) {
  return (n1 > n2) ? 1 : (n1 < n2) ? -1 : 0;
}

inline ulong uint64Hash(const uint64 &n) {
  return (((ulong*)&n)[0]) ^ (((ulong*)&n)[1]);
}

inline int uint64HashCmp(const uint64 &n1, const uint64 &n2) {
  return (n1 > n2) ? 1 : (n1 < n2) ? -1 : 0;
}

#ifdef IS64BIT
#define sizetHash    uint64Hash
#define sizetHashCmp uint64HashCmp
#else
#define sizetHash    uintHash
#define sizetHashCmp uintHashCmp
#endif

inline ulong floatHash(const float &f) {
  return (ulong)f;
}

inline int floatHashCmp(const float &f1, const float &f2) {
  return sign(f1 - f2);
}

inline ulong doubleHash(const double &d) {
  const ulong *p = (ulong*)(&d);
  return p[0] ^ p[1];
}

inline int doubleHashCmp(const double &d1, const double &d2) {
  return sign(d1 - d2);
}

template<typename T> ulong pointerHash(const T * const &p) {
  return sizetHash((size_t)p);
}

template<typename T> int pointerHashCmp(const T * const &p1, const T * const &p2) {
  return sizetHashCmp((size_t)p1, size_t(p2));
}

inline int boolCmp(bool b1, bool b2) {
  return ordinal(b1) - ordinal(b2);
}
