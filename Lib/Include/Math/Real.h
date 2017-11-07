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

inline Real getRealNaN() {
#ifdef LONGDOUBLE
  return Double80::DBL80_NAN;
#else
  return std::numeric_limits<double>::quiet_NaN();
#endif

}
// Return pointer to the character after parsing the string with the regular
// expression: {s}*[\-+]?({d}+|{d}+\.{d}*|{d}*\.{d}+)([eE][\-+]?{d}+)?
// where {d} = [0-9] and {s} = all characters c, where isspace(c) is true
// Return NULL if string is not recognized by the regular expression.
const _TUCHAR *parseReal(const _TUCHAR *s);

#include "PragmaLib.h"
