#include "pch.h"

ULONG striHash(const TCHAR * const &s) {
  ULONG result = 0;
  for(const _TUCHAR *cp = (const _TUCHAR*)s; *cp; cp++) {
    const _TUCHAR ch = _istupper(*cp) ? _totlower(*cp) : *cp;
    result = (result * 117) ^ ch;
  }
  return result;
}
