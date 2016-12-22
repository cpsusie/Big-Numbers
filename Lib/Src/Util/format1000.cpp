#include "pch.h"

#define DECORATEWITHDOTS(result, temp, n)     \
{ const int l = (int)_tcsclen(temp);          \
  int digitCount = (n < 0) ? (l-1) : l;       \
  if(digitCount <= 3) {                       \
    return temp;                              \
  }                                           \
                                              \
  TCHAR *src = temp;                          \
  TCHAR *dst = result;                        \
  if(*src == '-') {                           \
    *(dst++) = *(src++);                      \
  }                                           \
  while(*(dst++) = *(src++)) {                \
    if((digitCount--)%3 == 1 && digitCount) { \
      *(dst++) = '.';                         \
    }                                         \
  }                                           \
}

String format1000(INT n) {
  TCHAR temp[40];
  _itot(n,temp,10);
  TCHAR result[200];

  DECORATEWITHDOTS(result, temp, n);
  return result;
}

String format1000(UINT n) {
  TCHAR temp[40];
  _stprintf(temp, _T("%u"), n);
  TCHAR result[200];

  DECORATEWITHDOTS(result, temp, n);
  return result;
}

String format1000(LONG n) {
  return format1000((int)n);
}

String format1000(ULONG n) {
  return format1000((UINT)n);
}

String format1000(INT64 n) {
  TCHAR temp[70];
  _i64tot(n, temp, 10);
  TCHAR result[200];

  DECORATEWITHDOTS(result, temp, n);
  return result;
}

String format1000(UINT64 n) {
  TCHAR temp[70];
  _ui64tot(n, temp, 10);
  TCHAR result[200];

  DECORATEWITHDOTS(result, temp, n);
  return result;
}
