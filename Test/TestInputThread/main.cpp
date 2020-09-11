#include "stdafx.h"
#include <InputThread.h>

int main(int argc, char **argv) {
  InputThread input(stdin);
  int timeout = 5000;

  while(!input.endOfInput()) {
    try {
      const String line = input.getLine(timeout);
      if(line == _T("quit")) break;
      if(line == _T("kill")) input.kill();
      int newTimeout;
      if(_stscanf(line.cstr(), _T("timeout=%d"), &newTimeout) == 1) {
        timeout = newTimeout;
      }
      _tprintf(_T("read line \"%s\"\n"), line.cstr());
    } catch(TimeoutException e) {
      _tprintf(_T("Timeout:%s\n"), e.what());
    } catch(Exception e) {
      _tprintf(_T("Exception:%s\n"), e.what());
    }
  }
  return 0;
}
