#include "pch.h"

String formatSize(size_t n) {
#ifdef IS32BIT
  return format(_T("%lu"), n);
#else // IS64BIT
  return format(_T("%I64u"), n);
#endif // IS32BIT
}
