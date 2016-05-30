#include "pch.h"

static inline int translate(_TUCHAR ch, const unsigned char *table) {
  return (ch < 256)? table[ch] : ch;
}

#define TRANSLATE(ch) translate(ch, translateTable)

int strtabcmp(const TCHAR *s1, const TCHAR *s2, const unsigned char translateTable[256]) {
  const _TUCHAR *t1 = (const _TUCHAR*)s1;
  const _TUCHAR *t2 = (const _TUCHAR*)s2;
  int c = 0;
  while(*t1 && *t2 && (c = TRANSLATE(*(t1++)) - TRANSLATE(*(t2++))) == 0);
  if(*t1 && *t2) {
    return c;
  } else if(*t1 == 0 && *t2 == 0) {
    return c;
  } else if(*t1 == 0) {
    return -*t2;
  } else {
    return *t1;
  }
}

int strntabcmp(const TCHAR *s1, const TCHAR *s2, int n, const unsigned char translateTable[256]) {
  const _TUCHAR *t1 = (const _TUCHAR*)s1;
  const _TUCHAR *t2 = (const _TUCHAR*)s2;
  int c = 0;
  while(n && *t1 && *t2 && (c = TRANSLATE(*(t1++)) - TRANSLATE(*(t2++))) == 0) {
    n--;
  }
  if(n == 0) {
    return c;
  } else if(*t1 && *t2) {
    return c;
  } else if(*t1 == 0 && *t2 == 0) {
    return c;
  } else if(*t1 == 0) {
    return -*t2;
  } else {
    return *t1;
  }
}
