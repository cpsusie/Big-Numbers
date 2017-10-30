#include "pch.h"

bool readLine(FILE *f, String &str) {
  TCHAR line[4096];
  bool result = false;
  while(_fgetts(line, ARRAYSIZE(line), f)) {
    if(result) {
      str += line;
    } else {
      str = line;
    }
    result = true;
    size_t len = str.length();
    if((len > 0) && str.last() == _T('\n')) {
      str.removeLast(); len--;
      if((len > 0) && str.last() == _T('\r')) {
        str.removeLast();
      }
      break;
    }
  }
  return result;
}
