#include "stdafx.h"
#include "TestPTQueue.h"

int main() {
  try {
    testPtQueue();
  } catch(Exception e) {
    _tprintf(_T("Exception:%s\n"), e.what());
    return -1;
  }

  return 0;
}
