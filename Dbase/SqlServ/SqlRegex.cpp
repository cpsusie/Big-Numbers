#include "stdafx.h"
#include "SqlRegex.h"

String SqlRegex::convertWildCards(const String &wc) {
  // first replace sequences of %%% with one single %
  String str(wc);
  const TCHAR *s = wc.cstr();
  TCHAR       *t;
  bool lastWasPercent = false;
  for(t = str.cstr(); *s; s++) {
    if(*s == '%') {
      if(!lastWasPercent) {
        *(t++) = *s;
        lastWasPercent = true;
      }
    } else {
      *(t++) = *s;
      lastWasPercent = false;
    }
  }
  *t = 0;

  // then replace all % with '.*' and '_' with '.' 
  // and escape all 'special regex characters' with '\'
  TCHAR tmp[1024], *dst;
  s = str.cstr();
  for(dst = tmp; *s; s++) {
    if((dst - tmp) > ARRAYSIZE(tmp) - 3) {
      throwSqlError(SQL_LIKEPATTERN_TOO_LONG, _T("like-pattern <%s> too long"), wc);
    }
    switch(*s) {
    case '.' :
    case '\\':
    case '[' :
    case ']' :
    case '+' :
    case '?' :
      *(dst++) = '\\'; *(dst++) = *s;
      break;
    case '%' :
      *(dst++) = '.'; *(dst++) = '*';
      break;
    case '_':
      *(dst++) = '.';
      break;
    default  :
      *(dst++) = *s;
      break;
    }
  }
  *dst = 0;
//  _tprintf(_T("convertWildCards:<%s>\n"),tmp);
  return String(_T("\\`\\(")) + tmp + _T("\\)\\'");
}

void SqlRegex::compile(const String &pattern) {
  if(pattern == lastpattern) return;
  String pat(convertWildCards(pattern));
  try {
    Regex::compilePattern(pat, false);
    lastpattern = pattern;
  } catch(Exception e) { // should not come here
    throwSqlError(SQL_FATAL_ERROR,_T("Regex:%s"),e.what());
  }
}

bool SqlRegex::match(const String &str) const {
  return Regex::match(str);
}

bool SqlRegex::isLike(const String &str, const String &pattern) {
  compile(pattern);
  return match(str);
}
