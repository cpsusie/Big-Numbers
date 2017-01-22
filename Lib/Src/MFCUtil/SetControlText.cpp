#include "pch.h"

void setControlText(int id, CWnd *wnd) {
  const String title = getWindowText(wnd);
  if(title.length() > 0 && title[0] == '{') {
    String newTitle = loadString(id);
    wnd->SetWindowText(newTitle.cstr());
  }

  for(CWnd *child = wnd->GetWindow(GW_CHILD); child; child = child->GetNextWindow()) {
//    id = child->GetDlgCtrlID();
    const String text = getWindowText(child);
    if(text.length() > 0 && text[0] == '{') {
      const String label = loadString(child->GetDlgCtrlID());
      if(label.length() > 0) {
        setWindowText(child, label);
      }
    }
  }
}
/*
void substituteControl(int id, CWnd *wnd, CWnd &newCtrl) {
  CWnd      *oldCtrl = wnd->GetDlgItem(id);
  if(oldCtrl == NULL) {
    AfxMessageBox(format("Control with id=%d not found in window <%s>", id, getWindowText(wnd).cstr()).cstr(), MB_ICONWARNING);
    return;
  }
  const int style   = oldCtrl->GetStyle();
  const int exStyle = oldCtrl->GetExStyle();
  CRect     r       = getWindowRect(oldCtrl);
  String    text    = getWindowText(oldCtrl);

  oldCtrl->GetControlUnknown()GetIDispatch()CProgressCtrlCEdit
  oldCtrl->DestroyWindow();
  newCtrl.Create(style, r, wnd, id);
  newCtrl.ModifyStyleEx(0, exStyle);
  setWindowText(newCtrl, text);
}
*/
