#include "pch.h"
#include <WildCardRegex.h>

String WildCardRegex::convertWildCard(const TCHAR *wc) {
  TCHAR tmp[4096], *dst;
  for(dst = tmp; *wc; wc++) {
    switch(*wc) {
    case _T('.') :
    case _T('\\'):
    case _T('[') :
    case _T(']') :
    case _T('+') :
      *(dst++) = _T('\\'); *(dst++) = *wc;
      break;
    case '*' :
    case '?' :
      *(dst++) = _T('.'); *(dst++) = *wc;
      break;
    default  :
      *(dst++) = *wc;
      break;
    }
  }
  *dst = 0;
  return String(_T("\\(")) + tmp + _T("\\)");
}

void WildCardRegex::compile(const TCHAR **fnames) {
  String reg;
  for(int i = 0; *fnames; i++,fnames++) {
    if(i == 0) {
      reg = convertWildCard(*fnames);
    } else {
      reg += _T("\\|") + convertWildCard(*fnames);
    }
  }
  reg = String(_T("\\`\\(")) + reg + _T("\\)\\'");
  Regex::compilePattern(reg, String::upperCaseTranslate);
}

void WildCardRegex::compile(const TCHAR *str, bool ignoreCase, bool wholeWordOnly) {
  String reg = convertWildCard(str);
  if(wholeWordOnly) {
    reg = String(_T("\\<")) + reg + _T("\\>");
  }
  Regex::compilePattern(reg, ignoreCase ? String::upperCaseTranslate : nullptr);
}

void WildCardRegex::compile(const String &str, bool ignoreCase, bool wholeWordOnly) {
  compile(str.cstr(), ignoreCase, wholeWordOnly);
}
