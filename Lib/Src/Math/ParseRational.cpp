#include "pch.h"
#include <Math/Rational.h>

#define ISSIGN(ch) (((ch) == _T('-')) || ((ch) == _T('+')))
#define PARSEDIGITS(cp)  while(_istdigit(*cp)) { cp++; gotDigits = true; }

// Return pointer to the character after parsing the string with the regular
// expression: {s}*[\-+]?{d}+(/[\-+]?{d}+)?
// where {d} = [0-9] and {s} = all characters c, where isspace(c) is true
// Return NULL if string is not recognized by the regular expression.
const _TUCHAR *parseRational(const _TUCHAR *s) {
  bool gotDigits = false;
  while(_istspace(*s)) s++;
  if(ISSIGN(*s)) s++;
  PARSEDIGITS(s);
  if(*s == _T('/')) {
    s++;
    if(ISSIGN(*s)) s++;
    PARSEDIGITS(s);
  }
  return gotDigits ? s : NULL;
}
