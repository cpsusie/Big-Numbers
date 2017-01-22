#include "pch.h"

CompactIntArray getTabOrder(CWnd *wnd) {
  CompactIntArray idArray;
  for(CWnd *child = wnd->GetWindow(GW_CHILD); child; child = wnd->GetNextDlgTabItem(child)) {
    const int id = child->GetDlgCtrlID();
    if((idArray.size() > 0) && (id == idArray[0])) {
      break;
    }
    idArray.add(id);
  }
  return idArray;
}

void setTabOrder(CWnd *wnd, const CompactIntArray &tabOrderArray) {
  if(tabOrderArray.size() > 1) {
    const CWnd *prev = &CWnd::wndTop;
    for(size_t i = 0; i < tabOrderArray.size(); i++) {
      CWnd *next = wnd->GetDlgItem(tabOrderArray[i]);
      if(next) {
        next->SetWindowPos(prev, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        prev = next;
      }
    }
  }
}
