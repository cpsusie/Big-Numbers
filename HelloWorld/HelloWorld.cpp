// HelloWorld.cpp : main project file.

#include "stdafx.h"
#include <tchar.h>
#include <stdio.h>

int main(char **argv) {
  _tprintf(_T("sizeof(long double):%d\n"), sizeof(long double));
  for (int i = 0; i < 10; i++) {
		_tprintf(_T("Hello World %d\n"), i);
	}
  return 0;
}
