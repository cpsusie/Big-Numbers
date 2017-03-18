#include "pch.h"

String readTextFile(const String &fileName) {
  FILE *f = FOPEN(fileName,_T("r"));
  String result = readTextFile(f);
  fclose(f);
  return result;
}

String readTextFile(FILE *f) {
  String result, line;
  while(readLine(f, line)) {
    result += line;
    result += _T("\n");
  }
  return result;
}

