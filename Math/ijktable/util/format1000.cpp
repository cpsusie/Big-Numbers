#include "stdafx.h"

#define DECORATEWITHDOTS(result, temp, n)     \
{ const int l = (int)strlen(temp);            \
  int digitCount = (n < 0) ? (l-1) : l;       \
  if(digitCount <= 3) {                       \
    return temp;                              \
  }                                           \
                                              \
  char  *src = temp;                          \
  char  *dst = result;                        \
  if(*src == '-') {                           \
    *(dst++) = *(src++);                      \
  }                                           \
  while(*(dst++) = *(src++)) {                \
    if((digitCount--)%3 == 1 && digitCount) { \
      *(dst++) = '.';                         \
    }                                         \
  }                                           \
}

string format1000(int n) {
  char  temp[40];
  _itoa(n,temp,10);
  char  result[200];

  DECORATEWITHDOTS(result, temp, n);
  return result;
}

string format1000(uint n) {
  char  temp[40];
  sprintf(temp, "%u", n);
  char  result[200];

  DECORATEWITHDOTS(result, temp, n);
  return result;
}

string format1000(long n) {
  return format1000((int)n);
}

string format1000(ulong n) {
  return format1000((uint)n);
}

string format1000(int64 n) {
  char  temp[70];
  _i64toa(n, temp, 10);
  char  result[200];

  DECORATEWITHDOTS(result, temp, n);
  return result;
}

string format1000(uint64 n) {
  char  temp[70];
  _ui64toa(n, temp, 10);
  char  result[200];

  DECORATEWITHDOTS(result, temp, n);
  return result;
}
