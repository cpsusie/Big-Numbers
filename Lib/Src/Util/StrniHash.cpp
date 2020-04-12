#include "pch.h"

ULONG strniHash(const TCHAR * const &s, size_t n) {
  ULONG result = 0;
  for(const _TUCHAR *cp = s, *last = cp + n; cp < last; cp++) {
    const _TUCHAR ch = _istupper(*cp) ? _totlower(*cp) : *cp;
    result = (result * 117) ^ ch;
  }
  return result;
}

int strniHashCmp(const TCHAR * const &s1, const TCHAR * const &s2, size_t n1, size_t n2) {
  if(n1 == n2) {
    for(const _TUCHAR *cp1 = s1, *cp2 = s2, *last1 = cp1 + n1; cp1 < last1; cp1++, cp2++) {
      const int ch1 = _istupper(*cp1) ? _totlower(*cp1) : *cp1;
      const int ch2 = _istupper(*cp2) ? _totlower(*cp2) : *cp2;
      const int c   = ch1 - ch2;
      if(c) return c;
    }
    return 0; // lower(s1[0..n1]) == lower(s2[0..n1]). and same size
  } else if(n1 < n2) {
    for(const _TUCHAR *cp1 = s1, *cp2 = s2, *last1 = cp1 + n1; cp1 < last1; cp1++, cp2++) {
      const int ch1 = _istupper(*cp1) ? _totlower(*cp1) : *cp1;
      const int ch2 = _istupper(*cp2) ? _totlower(*cp2) : *cp2;
      const int c   = ch1 - ch2;
      if(c) return c;
    }
    return -1; // lower(s1[0..n1]) == lower(s2[0..n1]), but s2 is longer than s1
  } else { // (n1 > n2)
    for(const _TUCHAR *cp1 = s1, *cp2 = s2, *last2 = cp2 + n2; cp2 < last2; cp1++, cp2++) {
      const int ch1 = _istupper(*cp1) ? _totlower(*cp1) : *cp1;
      const int ch2 = _istupper(*cp2) ? _totlower(*cp2) : *cp2;
      const int c   = ch1 - ch2;
      if(c) return c;
    }
    return 1; // lower(s1[0..n2]) == lower(s2[0..n2]), but s1 is longer than s2
  }
}
