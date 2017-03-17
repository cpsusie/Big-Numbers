#include "stdafx.h"
#include "SqlRegex.h"

String SqlRegex::convertwc(const TCHAR *wc) {
  // first replace sequences of %%% with one single %
  String str(wc);
  const TCHAR *s = wc;
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
  *t = '\0';

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
  *dst = '\0';
//  _tprintf(_T("convertwc:<%s>\n"),tmp);
  return String(_T("\\`\\(")) + tmp + _T("\\)\\'");
}

void SqlRegex::compile(const TCHAR *pattern) {
  if(_tcscmp(pattern, lastpattern.cstr()) == 0) return;
  String pat(convertwc(pattern));
  try {
    Regex::compilePattern(pat.cstr(),false);
    lastpattern = pattern;
  } catch(Exception e) { // should not come here
    throwSqlError(SQL_FATAL_ERROR,_T("Regex:%s"),e.what());
  }
}

bool SqlRegex::match(const TCHAR *str) const {
  return Regex::match(str);
}

bool SqlRegex::isLike(const TCHAR *str, const TCHAR *pattern) {
  compile(pattern);
  return match(str);
}

#ifdef _DEBUG
void testSqlRegex() {
  for(;;) {
    _tprintf(_T("angiv pattern:"));
    TCHAR pattern[100];
    GETS(pattern);
    SqlRegex reg(pattern);
    reg.dump();
    for(;;) {
      TCHAR text[100];
      _tprintf("angiv text afslut med !:");
      GETS(text);
      if(text[0] == '!') break;
      if(reg.match(text)) {
        _tprintf(_T("match!\n"));
      } else {
        _tprintf(_T("no match!\n"));
      }

    }
  }
}
#endif
