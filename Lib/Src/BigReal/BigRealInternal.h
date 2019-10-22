#pragma once

//#define COUNT_CALLS

#ifdef COUNT_CALLS
#include <CallCounter.h>
#define DECLARE_CALLCOUNTER(name) static CallCounter _##name(_T(#name))
#define COUNTCALL(name)                              _##name.incr()
#else // COUNT_CALLS
#define DECLARE_CALLCOUNTER(name)
#define COUNTCALL(name)
#endif // COUNT_CALLS

#define VALIDATETOLERANCE(f)                \
  DEFINEMETHODNAME;                         \
  if(!f.isPositive()) {                     \
    throwInvalidToleranceException(method); \
  }

inline BigReal binop_inf(const BigReal &x, const BigReal &y, DigitPool *pool) {
  if(x._isfinite()) return BigReal(y,pool);
  if(y._isfinite()) return BigReal(x,pool);
  return isunordered(x,y) ? pool->nan() : x;
}

#define HANDLE_INFBINARYOP(x, y, pool)      \
  if(!x._isfinite() || !y._isfinite()) {    \
    return binop_inf(x, y, pool);           \
  }
