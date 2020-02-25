#include "stdafx.h"
#include "GotoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CGotoDlg::CGotoDlg(CWnd *pParent)   : CDialog(IDD, pParent) {
  m_line = 0;
}

void CGotoDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITLINENUMBER, m_line);
}

BEGIN_MESSAGE_MAP(CGotoDlg, CDialog)
END_MESSAGE_MAP()

BOOL CGotoDlg::OnInitDialog() {
  __super::OnInitDialog();

  gotoEditBox(this, IDC_EDITLINENUMBER);

  return FALSE;
}

void CGotoDlg::OnOK() {
  UpdateData();
  if(m_line < 1) {
    showWarning(_T("Please enter a linenumber > 0"));
  } else {
    __super::OnOK();
  }
}
