#include "stdafx.h"
#include <MyUtil.h>
#include <MFCUtil/WinTools.h>
#include <MFCUtil/ComboBoxWithHistory.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CComboBoxWithHistory::~CComboBoxWithHistory() {
}

BEGIN_MESSAGE_MAP(CComboBoxWithHistory, CComboBox)
	//{{AFX_MSG_MAP(CComboBoxWithHistory)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

DEFINECLASSNAME(CComboBoxWithHistory);

void CComboBoxWithHistory::substituteControl(CWnd *parent, int id, const String &registryName) {
  const TCHAR *method = _T("substituteControl");

  CComboBox *oldCtrl = (CComboBox*)parent->GetDlgItem(id);
  if(oldCtrl == NULL) {
    AfxMessageBox(format(_T("%s::%s:Control with id=%d does not exist"), s_className, method, id).cstr(), MB_ICONWARNING);
    return;
  }
  CRect wr;
  oldCtrl->GetDroppedControlRect(&wr);
  parent->ScreenToClient(&wr);
  const DWORD style   = oldCtrl->GetStyle();
  const DWORD exStyle = oldCtrl->GetExStyle();
  CFont *font = oldCtrl->GetFont();
  if(font == NULL) {
    font = parent->GetFont();
  }
  const CompactIntArray tabOrder = getTabOrder(parent);

  oldCtrl->DestroyWindow();

  if(!Create(style, wr, parent, id)) {
    AfxMessageBox(format(_T("%s::%s:Create failed"), s_className, method).cstr(),  MB_ICONWARNING);
    return;
  }
  setTabOrder(parent, tabOrder);

  ModifyStyleEx(0, exStyle);
  SetFont(font);
  m_history.setRegistryName(registryName);
  load();
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
  m_history.add((LPCTSTR)current);
  load();
  for(int i = 0; i < m_history.size(); i++) {
    if(m_history[i].cstr() == current) {
      SetCurSel(i);
      return;
    }
  }
}

void CComboBoxWithHistory::fillDropdownList() {
  ResetContent();
  for(int i = 0; i < m_history.size(); i++) {
    AddString(m_history[i].cstr());
  }
}

BOOL CComboBoxWithHistory::PreTranslateMessage(MSG* pMsg) {
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
    for(unsigned int i = 0; i < m_maxHistoryLength; i++) {
      String value;
      key.getValue(format(_T("s%02d"), i), value);
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
    const int n = min((int)m_maxHistoryLength, size());
    for(int i = 0; i < n; i++) {
      key.setValue(format(_T("s%02d"),i),(*this)[i]);
    }
  } catch(Exception) {
    // ignore
  }
}

bool ComboBoxHistory::add(const String &s) {
  for(int i = 0; i < size(); i++) {
    if((*this)[i] == s) {
      removeIndex(i);
      break;
    }
  }
  StringArray::add(0, s);
  while(size() > (int)m_maxHistoryLength) {
    removeLast();
  }
  return true;
}

void ComboBoxHistory::removeString(const String &str) {
  for(int i = 0; i < size(); i++) {
    if((*this)[i] == str) {
      removeIndex(i);
      return;
    }
  }
}

void ComboBoxHistory::setMaxHistoryLength(unsigned int maxLength) {
  m_maxHistoryLength = maxLength;
  while(size() > (int)m_maxHistoryLength) {
    removeLast();
  }
}
