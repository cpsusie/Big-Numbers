// testeventloglib.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\eventloglib\message.h"

int _tmain(int argc, TCHAR **argv) {
  eventLogInfo(_T("Hello World!"));
  return 0;
}
