#include "pch.h"

//#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

ULONG strHash(const TCHAR * const &s) {
  ULONG result = 0;
  for(const _TUCHAR *cp = (const _TUCHAR*)s; *cp;) {
    result = (result * 117) ^ *(cp++);
  }
  return result;
}
