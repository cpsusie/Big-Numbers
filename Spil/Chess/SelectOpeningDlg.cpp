#include "stdafx.h"
#include <MFCUtil/WinTools.h>
#include "SelectOpeningDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CSelectOpeningDlg::CSelectOpeningDlg(const StringArray &names, CWnd *pParent)
: m_names(names)
, CDialog(CSelectOpeningDlg::IDD, pParent) {
  m_selectedIndex = -1;
}

void CSelectOpeningDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSelectOpeningDlg, CDialog)
  ON_LBN_DBLCLK(IDC_LIST_NAMES, OnDblclkNamelist)
END_MESSAGE_MAP()

BOOL CSelectOpeningDlg::OnInitDialog() {
  __super::OnInitDialog();
  setControlText(IDD, this);

  CListBox *list = (CListBox*)GetDlgItem(IDC_LIST_NAMES);
  for(size_t i = 0; i < m_names.size(); i++) {
    list->AddString(m_names[i].cstr());
  }
  if(m_names.size() > 1) {
    list->SetSel(0);
  }
  list->SetFocus();
  return FALSE;
}

void CSelectOpeningDlg::OnOK() {
  CListBox *list = (CListBox*)GetDlgItem(IDC_LIST_NAMES);
  m_selectedIndex = list->GetCurSel();
  __super::OnOK();
}

void CSelectOpeningDlg::OnCancel() {
  m_selectedIndex = -1;

  __super::OnCancel();
}

const TCHAR *CSelectOpeningDlg::getSelectedName() const {
  return m_selectedIndex < 0 ? NULL : m_names[m_selectedIndex].cstr();
}

void CSelectOpeningDlg::OnDblclkNamelist() {
  OnOK();
}
