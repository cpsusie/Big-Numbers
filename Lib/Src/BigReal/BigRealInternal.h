#pragma once

#include <Math/BigReal/BigRealConfig.h>

//#define COUNT_CALLS

#if defined(COUNT_CALLS)
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

#if defined(assert)
#undef assert
#endif

#if defined(_DEBUG)
#define assert(exp) (void)( (exp) || (xassert(__TFILE__, __LINE__, _T(#exp)), 0) )
#else
#define assert(exp)
#endif

#if defined(TRACEPRODUCTRECURSION)
void logProductRecursion(UINT level, const TCHAR *method, _In_z_ _Printf_format_string_ const TCHAR * const format, ...);
#define LOGPRODUCTRECURSION(...) logProductRecursion(level, __TFUNCTION__, __VA_ARGS__)
#else
#define LOGPRODUCTRECURSION(...)
#endif // TRACEPRODUCTRECURSION
