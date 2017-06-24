#pragma once

#include <Unicode.h>
//#define LONGDOUBLE // defined in configurations "Debug long double" and "Release long double"

#ifdef LONGDOUBLE

#include "Double80.h"
typedef Double80 Real;
#else
typedef double Real;
#endif

typedef CompactArray<Real> CompactRealArray;

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

const _TUCHAR *parseReal(const _TUCHAR *s); // Return pointer to the character after parsing the string with the regular expression:
                                            // {s}*[\-+]?({d}+|{d}+\.{d}*|{d}*\.{d}+)([eE][\-+]?{d}+)?
                                            // where {d} = [0-9] and {s} = all characters c, where isspace(c) is true
                                            // Return NULL if string is not recognized by the regular expression.


#include "PragmaLib.h"
