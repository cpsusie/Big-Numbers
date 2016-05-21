#include "pch.h"

String toUpperCase(const String &str) {
  String result(str);
  strToUpperCase(result.cstr());
  return result;
}

String toLowerCase(const String &str) {
  String result(str);
  strToLowerCase(result.cstr());
  return result;
}

String firstLetterToUpperCase(const String &str) { // Return a copy of str, with first non-space letter changed to uppercase.
  String result(str);
  for(_TUCHAR *s = (_TUCHAR*)result.cstr(); _istspace(*s); s++);
  if(_istascii(*s) && _istlower(*s)) {
    *s = _totupper(*s);
  }
  return result;
}
