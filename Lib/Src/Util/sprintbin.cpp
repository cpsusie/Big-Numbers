#include "pch.h"

#define BITCOUNT(n) (sizeof(n)*8)
#define SPRINTBIN(n)                                    \
  TCHAR tmp[BITCOUNT(n)+1];                             \
  for(int index = BITCOUNT(n)-1; index >= 0; index--) { \
    tmp[index] = ((n) & 1) ? _T('1') : _T('0');         \
    n >>= 1;                                            \
  }                                                     \
  tmp[BITCOUNT(n)] = '\0';                              \
  return tmp;

String sprintbin(char c) {
  SPRINTBIN(c);
}

String sprintbin(unsigned char c) {
  SPRINTBIN(c);
}

String sprintbin(short s) {
  SPRINTBIN(s);
}

String sprintbin(unsigned short s) {
  SPRINTBIN(s);
}

String sprintbin(int i) {
  SPRINTBIN(i);
}

String sprintbin(UINT i) {
  SPRINTBIN(i);
}

String sprintbin(long l) {
  SPRINTBIN(l);
}

String sprintbin(unsigned long l) {
  SPRINTBIN(l);
}

String sprintbin(__int64 i) {
  SPRINTBIN(i);
}

String sprintbin(unsigned __int64 i) {
  SPRINTBIN(i);
}
