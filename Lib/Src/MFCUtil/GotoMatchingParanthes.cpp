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
    setCaretPos(edit, m);
  }
}

void gotoMatchingParanthes(CComboBox *cb) {
  DWORD sel = cb->GetEditSel();
  int startSel = sel & 0xffff, endSel = sel >> 16;
  const String str = getWindowText(cb);
  const int    m   = findMatchingpParanthes(str.cstr(), startSel);
  if(m >= 0) {
    cb->SetEditSel(m,m);
  }
}
