#include "pch.h"

void addData(CListCtrl &ctrl, int row, int col, const String &str, bool newItem) {
  LVITEM lv;
  lv.iItem    = row;
  lv.iSubItem = col;
  lv.pszText  = ((String&)str).cstr();
  lv.mask     = LVIF_TEXT;
  if(newItem) {
    ctrl.InsertItem(&lv);
  } else {
    ctrl.SetItem(&lv);   
  }
}

String getItemString(const CListCtrl &ctrl, int row, int col) {
  LVITEM lv;
  TCHAR tmp[4096];
  memset(&lv, 0, sizeof(lv));
  lv.iItem      = row;
  lv.iSubItem   = col;
  lv.pszText    = tmp;
  lv.cchTextMax = ARRAYSIZE(tmp);
  lv.mask       = LVIF_TEXT;
  if(ctrl.GetItem(&lv) == 0) {
    throwException(_T("getItemString(%d,%d) failed"), row, col);
  }
  return tmp;
}

void setSelectedIndex(CListCtrl &ctrl, int index) {
  if((ctrl.GetSelectionMark() != index) && (index >= 0)) {
    ctrl.SetSelectionMark(index);
    ctrl.SetItemState(index, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
    ctrl.EnsureVisible(index, TRUE);
  }
}

void filloutListHeaderWidth(CListCtrl &ctrl) {
  const int columnCount = ctrl.GetHeaderCtrl()->GetItemCount();
  int totalColumnWidth = 0;
  for(int i = 0; i < columnCount; i++) {
    totalColumnWidth += ctrl.GetColumnWidth(i);
  }
  const int clientWidth = getClientRect(&ctrl).Size().cx;
  for(int missing = clientWidth - totalColumnWidth; missing > 0;) {
    for(int i = 0, dw = max(1, missing/columnCount); i < columnCount && missing > 0; i++, missing -= dw) {
      ctrl.SetColumnWidth(i, ctrl.GetColumnWidth(i) + dw);
    }
  }
}

