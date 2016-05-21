#include "stdafx.h"
#include <MyUtil.h>
#include "TestByteArray.h"

int _tmain(int argc, TCHAR **argv) {
  try {
    testByteArray();
  } catch(Exception e) {
    _ftprintf(stderr, _T("Exception:%s\n"), e.what());
    return -1;
  }
  return 0;
}
