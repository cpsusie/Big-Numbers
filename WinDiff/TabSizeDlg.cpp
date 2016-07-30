#include "stdafx.h"
#include "TabSizeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CTabSizeDlg::CTabSizeDlg(int tabSize, CWnd *pParent) : CDialog(CTabSizeDlg::IDD, pParent) {
  m_tabSize = tabSize;
}

void CTabSizeDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITTABSIZE, m_tabSize);
  DDV_MinMaxUInt(pDX, m_tabSize, 0, 20);
}


BEGIN_MESSAGE_MAP(CTabSizeDlg, CDialog)
END_MESSAGE_MAP()

void CTabSizeDlg::OnOK() {
  UpdateData();
  CDialog::OnOK();
}

BOOL CTabSizeDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  gotoEditBox(this, IDC_EDITTABSIZE);
  return FALSE;
}
