#pragma once

#if defined(ARRAYSIZE)
#undef ARRAYSIZE
#endif

#define ARRAYSIZE(a)        (sizeof(a)/sizeof((a)[0]))
#define LASTVALUE(a)        (a[ARRAYSIZE(a)-1])

inline int ordinal(bool b) {
  return b ? 1 : 0;
}

// Set all pointers in a, [0..size-1] to nullptr
// Return a
template<typename S> S **resetPointerArray(S **a, size_t size) {
  if(size > 0) {
    memset(a, 0, sizeof(a[0]) * size);
  }
  return a;
}
