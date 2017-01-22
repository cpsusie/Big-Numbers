#include "pch.h"

String readFile(const String &fileName) {
  FILE *f = FOPEN(fileName,"r");
  String result = readFile(f);
  fclose(f);
  return result;
}

String readFile(FILE *f) {
  String result;
  String line;
  while(readLine(f,line)) {
    result += line;
    result += _T("\n");
  }
  return result;
}

bool readLine(FILE *f, String &str) {
  TCHAR line[4096];
  str = _T("");
  bool result = false;
  while(_fgetts(line, ARRAYSIZE(line), f)) {
    result = true;
    str += line;
    int last = (int)str.length()-1;
    if((last >= 0) && str[last] == _T('\n')) {
      str.remove(last--);
      if((last >= 0) && str[last] == _T('\r')) {
        str.remove(last);
      }
      break;
    }
  }
  return result;
}

