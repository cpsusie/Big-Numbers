#include "stdafx.h"
#include <Scanner.h>

int main(int argc, char **argv) {
  LexFileStream s;

//  s.open(_T("c:\\temp\\UnicodeUTF8A"));
    s.open(_T("c:\\temp\\unicodeText"));
//  s.open(_T("c:\\mytools2015\\ShowGraf\\ShowGraf.rc"));
//  s.open(_T("c:\\mytools2015\\D3FunctionPlotter\\D3FunctionPlotter.rc"));
  while (!s.eof()) {
    _TUCHAR buf[100];
    intptr_t n = s.getChars(buf, ARRAYSIZE(buf) - 1);
    buf[n] = _T('\0');
    String str(buf);
    _tprintf(_T("%s"), str.cstr());
  }
  return 0;
}

