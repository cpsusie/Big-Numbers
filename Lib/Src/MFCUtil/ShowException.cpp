#include "stdafx.h"
#include <MFCUtil/WinTools.h>

void showException(const Exception &e) {
  AfxMessageBox(format(_T("%s"), e.what()).cstr(), MB_ICONWARNING);
}

