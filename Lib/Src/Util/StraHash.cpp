#include "pch.h"

ULONG strHash(const char * const &s) {
  ULONG result = 0;
  for(const char *cp = s; *cp;) {
    result = (result * 117) ^ (unsigned char)(*(cp++));
  }
  return result;
}
