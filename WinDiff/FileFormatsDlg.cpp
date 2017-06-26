#include "stdafx.h"
#include "DefineFileFormatDlg.h"
#include "FileFormatsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CFileFormatsDlg::CFileFormatsDlg(CWnd *pParent) : CDialog(CFileFormatsDlg::IDD, pParent) {
}

void CFileFormatsDlg::DoDataExchange(CDataExchange* pDX) {
  __super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LISTFORMATNAMES, m_nameListCtrl);
}

BEGIN_MESSAGE_MAP(CFileFormatsDlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTONRENAME, OnButtonRename)
    ON_BN_CLICKED(IDC_BUTTONDELETE, OnButtonDelete)
    ON_NOTIFY(LVN_ITEMCHANGED       , IDC_LISTFORMATNAMES, OnItemChangedList )
    ON_NOTIFY(LVN_ENDLABELEDIT      , IDC_LISTFORMATNAMES, OnEndLabelEditList)
END_MESSAGE_MAP()

BOOL CFileFormatsDlg::OnInitDialog() {
  __super::OnInitDialog();
    
  m_nameListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT);
  m_nameListCtrl.InsertColumn( 0,EMPTYSTRING,LVCFMT_LEFT, 200);
  updateListCtrl();
  GetDlgItem(IDC_LISTFORMATNAMES)->SetFocus();
  setSelectedIndex(m_nameListCtrl, 0);
  ajourButtons();
  m_origList = m_nameList;
  return false;
}

void CFileFormatsDlg::updateListCtrl() {
  m_nameListCtrl.DeleteAllItems();
  for(size_t i = 0; i < m_nameList.size(); i++) {
    FileFormatEditData &ff = m_nameList[i];
    addData(m_nameListCtrl, (int)i, 0, ff.m_name, true);
  }
  Invalidate(FALSE);
}

int CFileFormatsDlg::getSelectedIndex() {
  return m_nameListCtrl.GetSelectionMark();
}

int CFileFormatsDlg::getListSize() {
  return m_nameListCtrl.GetItemCount();
}

void CFileFormatsDlg::OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult) {
  NM_LISTVIEW &listView = *(NM_LISTVIEW*)pNMHDR;
  if((listView.uChanged & LVIF_STATE) && (listView.uNewState & LVNI_SELECTED)) {
    ajourButtons(listView.iItem);
  }
}

void CFileFormatsDlg::OnEndLabelEditList(NMHDR* pNMHDR, LRESULT* pResult) {
  LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

  LVITEM &lv = pDispInfo->item;
  const TCHAR *str = lv.pszText;
  if(str == NULL || str[0] == 0) {
    *pResult = FALSE;
  } else {
    *pResult = TRUE;
    m_nameList[getSelectedIndex()].m_name = str;
  }
}

void CFileFormatsDlg::ajourButtons(int selected) {
  if(selected < 0) {
    selected = getSelectedIndex();
  }
  GetDlgItem(IDC_BUTTONRENAME  )->EnableWindow((selected >= 0             )?TRUE:FALSE);
  GetDlgItem(IDC_BUTTONDELETE  )->EnableWindow((selected >= 0             )?TRUE:FALSE);
}

bool CFileFormatsDlg::anythingChanged() const {
  return m_nameList != m_origList;
}

void CFileFormatsDlg::OnOK() {
  if(anythingChanged()) {
    int errorLine;
    if(!m_nameList.validate(errorLine)) {
      setSelectedIndex(m_nameListCtrl, errorLine);
      return;
    }
    if(!m_nameList.save()) {
      return;
    }
    if(m_nameList.size() == 0) {
      OnCancel();
    }
  }
  const int selected = getSelectedIndex();
  if(selected >= 0) {
    m_selected = m_nameList[selected].m_name;
  }
  __super::OnOK();
}

void CFileFormatsDlg::OnButtonRename() {
  m_nameListCtrl.SetFocus();
  m_nameListCtrl.EditLabel(getSelectedIndex());
}

void CFileFormatsDlg::OnButtonDelete() {
  const int selected = getSelectedIndex();
  const bool wasLast = selected == getListSize()-1;
  m_nameList.remove(selected);
  updateListCtrl();
  if(wasLast && (selected > 0)) {
    setSelectedIndex(m_nameListCtrl, selected-1);
  }
}
