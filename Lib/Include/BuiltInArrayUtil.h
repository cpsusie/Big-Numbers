#pragma once

#if defined(ARRAYSIZE)
#undef ARRAYSIZE
#endif

#define ARRAYSIZE(a)        (sizeof(a)/sizeof((a)[0]))
#define LASTVALUE(a)        (a[ARRAYSIZE(a)-1])

inline int ordinal(bool b) {
  return b ? 1 : 0;
}
