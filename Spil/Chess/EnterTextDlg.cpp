#include "stdafx.h"
#include <MFCUtil/WinTools.h>
#include "EnterTextDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CEnterTextDlg::CEnterTextDlg(const String &str, CWnd* pParent) : CDialog(CEnterTextDlg::IDD, pParent) {
  m_text = str.cstr();
}


void CEnterTextDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDIT_TEXTBOX, m_text);
}


BEGIN_MESSAGE_MAP(CEnterTextDlg, CDialog)
END_MESSAGE_MAP()


void CEnterTextDlg::OnOK() {
  UpdateData();
  CDialog::OnOK();
}

BOOL CEnterTextDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  setControlText(IDD, this);
  gotoEditBox(this, IDC_EDIT_TEXTBOX);
  return FALSE;
}
