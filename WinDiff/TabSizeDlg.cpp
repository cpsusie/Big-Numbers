#include "stdafx.h"
#include "TabSizeDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CTabSizeDlg::CTabSizeDlg(int tabSize, CWnd *pParent) : CDialog(IDD, pParent) {
  m_tabSize = tabSize;
}

void CTabSizeDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITTABSIZE, m_tabSize);
  DDV_MinMaxUInt(pDX, m_tabSize, 0, 20);
}

BEGIN_MESSAGE_MAP(CTabSizeDlg, CDialog)
END_MESSAGE_MAP()

void CTabSizeDlg::OnOK() {
  UpdateData();
  __super::OnOK();
}

BOOL CTabSizeDlg::OnInitDialog() {
  __super::OnInitDialog();
  gotoEditBox(this, IDC_EDITTABSIZE);
  return FALSE;
}
