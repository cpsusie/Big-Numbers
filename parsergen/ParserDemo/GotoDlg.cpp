#include "stdafx.h"
#include "GotoDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

GotoDlg::GotoDlg(CWnd *pParent) : CDialog(GotoDlg::IDD, pParent) {
  m_line = 0;
}

void GotoDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITLINENUMBER, m_line);
}

BEGIN_MESSAGE_MAP(GotoDlg, CDialog)
END_MESSAGE_MAP()

BOOL GotoDlg::OnInitDialog() {
  __super::OnInitDialog();
  gotoEditBox(this, IDC_EDITLINENUMBER);
  return FALSE;
}

void GotoDlg::OnOK() {
  UpdateData();	
  if(m_line < 1) {
    showWarning(_T("Please enter a linenumber > 0"));
  } else {
    __super::OnOK();
  }
}
