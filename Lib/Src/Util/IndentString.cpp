#include "pch.h"
#include <Tokenizer.h>

String indentString(const String &s, UINT n) {
  if(n == 0) return s;
  const String linePrefix = spaceString(n);
  String result;
  for(Tokenizer tok(s, _T("\n\r")); tok.hasNext();) {
    result += linePrefix;
    result += tok.next();
    result += _T("\n");
  }
  return result;
}
