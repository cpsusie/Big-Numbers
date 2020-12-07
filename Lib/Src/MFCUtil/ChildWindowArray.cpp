#include "pch.h"

static BOOL CALLBACK childWindowArrayEnumerationProc(_In_ HWND wnd, _In_ LPARAM lParam) {
  ChildWindowArray &a = *(ChildWindowArray*)lParam;
  if(a.m_recursive || (GetParent(wnd) == a.m_parent)) {
    const int id = GetDlgCtrlID(wnd);
    if(id == 0) {
      throwLastErrorOnSysCallException(__TFUNCTION__, _T("GetDlgCtrlID"));
    }
    a.add((int)id);
  }
  return TRUE;
}

void ChildWindowArray::enumerateChildren(HWND parent, bool recursive) {
  clear(-1);
  m_parent    = parent;
  m_recursive = recursive;
  EnumChildWindows(m_parent, childWindowArrayEnumerationProc, (LPARAM)this);
}

int ChildWindowArray::getChildIndex(HWND child) const {
  if(!m_recursive && (GetParent(child) != m_parent)) {
    return -1;
  }
  return getChildIndex(GetDlgCtrlID(child));
}
