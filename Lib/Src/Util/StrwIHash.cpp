#include "pch.h"

ULONG striHash(const wchar_t * const &s) {
  ULONG result = 0;
  for(const unsigned short *cp = (const unsigned short*)s; *cp; cp++) {
    const unsigned int ch = iswupper(*cp) ? towlower(*cp) : *cp;
    result = (result * 117) ^ ch;
  }
  return result;
}
