#include "pch.h"
#include <Math/Real.h>

#define ISSIGN(ch) (((ch) == _T('-')) || ((ch) == _T('+')))
#define PARSEDIGITS(cp)  while(_istdigit(*cp)) { cp++; gotDigits = true; }

const _TUCHAR *parseReal(const _TUCHAR *s) {
  bool gotDigits = false;
  while(_istspace(*s)) s++;
  if(ISSIGN(*s)) s++;
  PARSEDIGITS(s);
  if(*s == _T('.') && _istdigit(s[1])) {
    s++;
    PARSEDIGITS(s);
  }
  if(gotDigits) {
    if(*s == _T('e') || *s == _T('E')) {
      s++;
      if(ISSIGN(*s)) s++;
      PARSEDIGITS(s);
    }
  }
  return gotDigits ? s : NULL;
}
