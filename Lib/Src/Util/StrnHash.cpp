#include "pch.h"

ULONG strnHash(const TCHAR * const &s, size_t n) {
  ULONG result = 0;
  for(const _TUCHAR *cp = s, *last = cp + n; cp < last;) {
    result = (result * 117) ^ *(cp++);
  }
  return result;
}

int strnHashCmp(const TCHAR * const &s1, const TCHAR * const &s2, size_t n1, size_t n2) {
  if (n1 == n2) {
    for (const _TUCHAR *cp1 = s1, *cp2 = s2, *last1 = cp1 + n1; cp1 < last1;) {
      const int c = (int)*(cp1++) - (int)*(cp2++);
      if(c) return c;
    }
    return 0; // s1[0..n1] == s2[0..n1]. and same size
  } else if(n1 < n2) {
    for(const _TUCHAR *cp1 = s1, *cp2 = s2, *last1 = cp1 + n1; cp1 < last1;) {
      const int c = (int)*(cp1++) - (int)*(cp2++);
      if(c) return c;
    }
    return -1; // s1[0..n1] == s2[0..n1], but s2 is longer than s1
  } else { // (n1 > n2)
    for(const _TUCHAR *cp1 = s1, *cp2 = s2, *last2 = cp2 + n2; cp2 < last2;) {
      const int c = (int)*(cp1++) - (int)*(cp2++);
      if(c) return c;
    }
    return 1; // s1[0..n2] == s2[0..n2], but s1 is longer than s2
  }
}
