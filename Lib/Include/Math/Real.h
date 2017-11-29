#pragma once

#include <Unicode.h>
#include <NumberInterval.h>
//#define LONGDOUBLE // defined in configurations "Debug long double" and "Release long double"

#ifdef LONGDOUBLE

#include "Double80.h"
typedef Double80 Real;
#else
typedef double Real;
#endif

typedef CompactArray<Real>   CompactRealArray;
typedef NumberInterval<Real> RealInterval;

inline double getDouble(double x) {
  return x;
}
inline float getFloat(double x) {
  return (float)x;
}
inline int getInt(double x) {
  return (int)x;
}
inline UINT getUint(double x) {
  return (UINT)x;
}
inline long getLong(double x) {
  return getInt(x);
}
inline ULONG getUlong(double x) {
  return getUint(x);
}
inline INT64 getInt64(double x) {
  return (INT64)x;
}
inline UINT64 getUint64(double x) {
  return (UINT64)x;
}

inline bool isFloat(double x) {
  return x == (float)x;
}
inline bool isInt64(double x) {
  return x == getInt64(x);
}
inline bool isInt(double x) {
  return x == getInt(x);
}

inline Real getRealNaN() {
#ifdef LONGDOUBLE
  return Double80::DBL80_NAN;
#else
  return std::numeric_limits<double>::quiet_NaN();
#endif

}

#ifndef LONGDOUBLE
#define strtor strtod
#define wcstor wcstod
#else // LONGDOUBLE
#define strtor strtod80
#define wcstor wcstod80
#endif // LONGDOUBLE

#ifdef _UNICODE
#define _tcstor wcstor
#else
#define _tcstor strtor
#endif // _UNICODE

#include "PragmaLib.h"
