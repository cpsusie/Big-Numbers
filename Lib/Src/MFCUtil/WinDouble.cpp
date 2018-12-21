#include "pch.h"

bool getWindowDouble(CWnd *wnd, int id, double &d) {
  const String str = getWindowText(wnd, id);
  if(_stscanf(str.cstr(),_T("%le"), &d) != 1) {
    gotoEditBox(wnd, id);
    showWarning(_T("Expected number"));
    return false;
  }
  return true;
}

