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

#ifdef assert
#undef assert
#endif

#ifdef _DEBUG
#define assert(exp) (void)( (exp) || (xassert(__TFILE__, __LINE__, _T(#exp)), 0) )
#else
#define assert(exp)
#endif
