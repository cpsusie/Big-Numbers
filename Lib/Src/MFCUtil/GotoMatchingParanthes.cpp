#include "pch.h"

void gotoMatchingParanthes(CWnd *wnd, int id) {
  gotoMatchingParanthes((CEdit*)wnd->GetDlgItem(id));
}

void gotoMatchingParanthes(CEdit *edit) {
  int cursorPos, endChar;
  edit->GetSel(cursorPos,endChar);
  const String str = getWindowText(edit);
  const int    m   = findMatchingpParanthes(str.cstr(), cursorPos);
  if(m >= 0) {
    edit->SetSel(m, m);
  }
}
