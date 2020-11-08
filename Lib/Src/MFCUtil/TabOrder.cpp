#include "pch.h"

void getTabOrder(HWND dlg, CompactUIntArray &tabOrder) {
  tabOrder.clear(100);
  for(HWND child = GetNextDlgTabItem(dlg, HWND_TOP, FALSE); child != nullptr; child = GetNextDlgTabItem(dlg, child, FALSE)) {
    const int id = GetDlgCtrlID(child);
    if(!tabOrder.isEmpty() && (id == tabOrder[0])) {
      break;
    }
    tabOrder.add(id);
  }
}

void setTabOrder(HWND dlg, const CompactUIntArray &tabOrder) {
  if(tabOrder.size() > 1) {
    HWND prev = HWND_TOP;
    for(ConstIterator<UINT> it = tabOrder.getIterator(); it.hasNext();) {
      const UINT id = it.next();
      HWND wnd = GetDlgItem(dlg, id);
      if(wnd) {
        SetWindowPos(wnd, prev, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        prev = wnd;
      }
    }
  }
}
