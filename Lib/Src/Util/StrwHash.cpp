#include "pch.h"

ULONG strHash(const wchar_t * const &s) {
  ULONG result = 0;
  for(const wchar_t *cp = s; *cp;) {
    result = (result * 117) ^ (unsigned short)(*(cp++));
  }
  return result;
}
