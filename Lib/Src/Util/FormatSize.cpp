#include "pch.h"

String formatSize(size_t n) {
#if defined(IS32BIT)
  return format(_T("%lu"), n);
#else // IS64BIT
  return format(_T("%I64u"), n);
#endif // IS32BIT
}
