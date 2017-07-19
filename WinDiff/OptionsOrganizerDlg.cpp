#include "stdafx.h"
#include "WinDiff.h"
#include "OptionsOrganizerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

COptionsOrganizerDlg::COptionsOrganizerDlg(CWnd *pParent /*=NULL*/)
    : CDialog(COptionsOrganizerDlg::IDD, pParent)
{
}

void COptionsOrganizerDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LISTOPTIONNAMES, m_nameListCtrl);
}

BEGIN_MESSAGE_MAP(COptionsOrganizerDlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTONRENAME  , OnButtonRename    )
    ON_BN_CLICKED(IDC_BUTTONDELETE  , OnButtonDelete)
    ON_BN_CLICKED(IDC_BUTTONMOVEDOWN, OnButtonMoveDown)
    ON_BN_CLICKED(IDC_BUTTONMOVEUP  , OnButtonMoveUp)
    ON_NOTIFY(LVN_ITEMCHANGED       , IDC_LISTOPTIONNAMES, OnItemChangedList )
    ON_NOTIFY(LVN_ENDLABELEDIT      , IDC_LISTOPTIONNAMES, OnEndLabelEditList)
END_MESSAGE_MAP()

BOOL COptionsOrganizerDlg::OnInitDialog() {
  __super::OnInitDialog();

  m_nameListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT);
  m_nameListCtrl.InsertColumn( 0,EMPTYSTRING,LVCFMT_LEFT, 200);
  updateListCtrl();
  GetDlgItem(IDC_LISTOPTIONNAMES)->SetFocus();
  setSelectedIndex(m_nameListCtrl, 0);
  ajourButtons();

  return FALSE;
}

void COptionsOrganizerDlg::updateListCtrl() {
  m_nameListCtrl.DeleteAllItems();
  for(size_t i = 0; i < m_nameList.size(); i++) {
    OptionsEditData &opt = m_nameList[i];
    addData(m_nameListCtrl, (int)i, 0, opt.m_name, true);
  }
  Invalidate(FALSE);
}

int COptionsOrganizerDlg::getSelectedIndex() {
  return m_nameListCtrl.GetSelectionMark();
}

int COptionsOrganizerDlg::getListSize() {
  return m_nameListCtrl.GetItemCount();
}

void COptionsOrganizerDlg::OnItemChangedList(NMHDR *pNMHDR, LRESULT *pResult) {
  NM_LISTVIEW &listView = *(NM_LISTVIEW*)pNMHDR;
  if ((listView.uChanged & LVIF_STATE) && (listView.uNewState & LVNI_SELECTED)) {
    ajourButtons(listView.iItem);
  }
}

void COptionsOrganizerDlg::OnEndLabelEditList(NMHDR *pNMHDR, LRESULT *pResult) {
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

void COptionsOrganizerDlg::ajourButtons(int selected) {
  if(selected < 0) {
    selected = getSelectedIndex();
  }
  GetDlgItem(IDC_BUTTONRENAME  )->EnableWindow((selected >= 0             )?TRUE:FALSE);
  GetDlgItem(IDC_BUTTONDELETE  )->EnableWindow((selected >= 0             )?TRUE:FALSE);
  GetDlgItem(IDC_BUTTONMOVEUP  )->EnableWindow((selected >  0             )?TRUE:FALSE);
  GetDlgItem(IDC_BUTTONMOVEDOWN)->EnableWindow((selected >= 0)
                                            && (selected < getListSize()-1)?TRUE:FALSE);
}

void COptionsOrganizerDlg::OnOK() {
  int errorLine;
  if(!m_nameList.validate(errorLine)) {
    setSelectedIndex(m_nameListCtrl, errorLine);
    return;
  }
  if(m_nameList.save()) {
    __super::OnOK();
  }
}

void COptionsOrganizerDlg::OnCancel() {
  __super::OnCancel();
}

void COptionsOrganizerDlg::OnButtonRename() {
  const int selected = getSelectedIndex();
  if(selected >= 0) {
    m_nameListCtrl.SetFocus();
    m_nameListCtrl.EditLabel(selected);
  }
}

void COptionsOrganizerDlg::OnButtonDelete() {
  const int selected = getSelectedIndex();
  if(selected >= 0) {
    const bool wasLast = selected == getListSize()-1;
    m_nameList.remove(selected);
    updateListCtrl();
    if(wasLast && (selected > 0)) {
      setSelectedIndex(m_nameListCtrl, selected-1);
    }
  }
}


void COptionsOrganizerDlg::OnButtonMoveUp() {
  const int selected = getSelectedIndex();
  if(selected > 0) {
    if(m_nameList.move(selected, true)) {
      updateListCtrl();
      setSelectedIndex(m_nameListCtrl, selected-1);
    }
  }
}

void COptionsOrganizerDlg::OnButtonMoveDown() {
  const int selected = getSelectedIndex();
  if(selected >= 0) {
    if(m_nameList.move(selected, false)) {
      updateListCtrl();
      setSelectedIndex(m_nameListCtrl, selected+1);
    }
  }
}
