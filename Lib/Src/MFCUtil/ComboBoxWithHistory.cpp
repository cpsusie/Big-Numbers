#include "pch.h"
#include <MyUtil.h>
#include <FileNameSplitter.h>
#include <MFCUtil/ComboBoxWithHistory.h>

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CComboBoxWithHistory::~CComboBoxWithHistory() {
}

BEGIN_MESSAGE_MAP(CComboBoxWithHistory, CComboBox)
END_MESSAGE_MAP()

void CComboBoxWithHistory::substituteControl(CWnd *parent, int id, const String &registryName) {
  DEFINEMETHODNAME;

  CComboBox *oldCtrl = (CComboBox*)parent->GetDlgItem(id);
  if(oldCtrl == nullptr) {
    showError(_T("%s:Control with id=%d does not exist"), method, id);
    return;
  }
  CRect wr;
  oldCtrl->GetDroppedControlRect(&wr);
  parent->ScreenToClient(&wr);
  const DWORD style   = oldCtrl->GetStyle();
  const DWORD exStyle = oldCtrl->GetExStyle();
  CFont *font = oldCtrl->GetFont();
  if(font == nullptr) {
    font = parent->GetFont();
  }
  const TabOrder tabOrder(parent);

  oldCtrl->DestroyWindow();

  if(!Create(style, wr, parent, id)) {
    showError(_T("%s:Create failed"), method);
    return;
  }
  tabOrder.restoreTabOrder();

  ModifyStyleEx(0, exStyle);
  SetFont(font);
  m_history.setRegistryName(registryName);
  load();
  SetCurSel(-1);
}

void CComboBoxWithHistory::load() {
  fillDropdownList();
}

void CComboBoxWithHistory::save() {
  m_history.save();
}

void CComboBoxWithHistory::updateList() {
  CString current;
  GetWindowText(current);
  DWORD sel = GetEditSel();
  m_history.add((LPCTSTR)current);
  load();
  for(UINT i = 0; i < m_history.size(); i++) {
    if(m_history[i].cstr() == current) {
      SetCurSel(i);
      SetEditSel(LOWORD(sel),HIWORD(sel));
      return;
    }
  }
}

void CComboBoxWithHistory::fillDropdownList() {
  ResetContent();
  for(size_t i = 0; i < m_history.size(); i++) {
    AddString(m_history[i].cstr());
  }
}

BOOL CComboBoxWithHistory::PreTranslateMessage(MSG *pMsg) {
  if(GetDroppedState()) {
    switch(pMsg->message) {
    case WM_KEYDOWN:
      switch(pMsg->wParam) {
      case VK_DELETE:
        { const int curSel = GetCurSel();
          CString str ;
          GetLBText(curSel, str);
          DeleteString(curSel);
          if(curSel == GetCount()) {
            SetCurSel(curSel-1);
          } else {
            SetCurSel(curSel);
          }
          m_history.remove((LPCTSTR)str);
        }
        return TRUE;
      }
    }
  }
  return CComboBox::PreTranslateMessage(pMsg);
}

static RegistryKey getRootKey() { // static
  static String rootPath;
  if(rootPath.length() == 0) {
    const FileNameSplitter info(getModuleFileName());
    rootPath = FileNameSplitter::getChildName(_T("JGMData"), info.getFileName());
  }
  return RegistryKey(HKEY_CURRENT_USER, _T("Software")).createOrOpenPath(rootPath);
}

static RegistryKey getSubKey(const String &name) {
  return getRootKey().createOrOpenKey(name);
}

RegistryKey ComboBoxHistory::getKey() {
  return getSubKey(m_registryName);
}

ComboBoxHistory::~ComboBoxHistory() {
  save();
}

void ComboBoxHistory::setRegistryName(const String &name) {
  m_registryName = name;
  load();
}

void ComboBoxHistory::load() {
  try {
    RegistryKey key = getKey();
    clear();
    for(UINT i = 0; i < m_maxHistoryLength; i++) {
      String value;
      key.getValue(format(_T("s%02u"), i), value);
      if(value.length() > 0) {
        StringArray::add(value);
      }
    }
  } catch(Exception) {
    // ignore
  }
}

void ComboBoxHistory::save() {
  try {
    RegistryKey key = getKey();
    key.deleteValues();
    const UINT n = min(m_maxHistoryLength, (UINT)size());
    for(UINT i = 0; i < n; i++) {
      key.setValue(format(_T("s%02u"),i),(*this)[i]);
    }
  } catch(Exception) {
    // ignore
  }
}

bool ComboBoxHistory::add(const String &s) {
  for(size_t i = 0; i < size(); i++) {
    if((*this)[i] == s) {
      removeIndex(i);
      break;
    }
  }
  StringArray::insert(0, s);
  while(size() > m_maxHistoryLength) {
    removeLast();
  }
  return true;
}

void ComboBoxHistory::removeString(const String &str) {
  for(size_t i = 0; i < size(); i++) {
    if((*this)[i] == str) {
      removeIndex(i);
      return;
    }
  }
}

void ComboBoxHistory::setMaxHistoryLength(UINT maxLength) {
  m_maxHistoryLength = maxLength;
  while(size() > m_maxHistoryLength) {
    removeLast();
  }
}
