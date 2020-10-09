#include "pch.h"

ULONG striHash(const char * const &s) {
  ULONG result = 0;
  for(const unsigned char *cp = (const unsigned char*)s; *cp; cp++) {
    const unsigned int ch = isupper(*cp) ? tolower(*cp) : *cp;
    result = (result * 117) ^ ch;
  }
  return result;
}
