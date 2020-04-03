#include "stdafx.h"

string formatSize(size_t n) {
#ifdef IS32BIT
  return format("%lu", n);
#else // IS64BIT
  return format("%I64u", n);
#endif // IS32BIT
}
