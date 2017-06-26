#include "stdafx.h"
#include "regnetester.h"
#include "PasswordDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPasswordDlg::CPasswordDlg(const String &expectedPassword, CWnd *pParent /*=NULL*/)
    : CDialog(CPasswordDlg::IDD, pParent)
{
    m_password = EMPTYSTRING;
    m_expectedPassword = expectedPassword.cstr();
}


void CPasswordDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDITPASSWORD, m_password);
}


BEGIN_MESSAGE_MAP(CPasswordDlg, CDialog)
END_MESSAGE_MAP()

void CPasswordDlg::OnOK() {
  UpdateData();
  if(m_password != m_expectedPassword) {
    MessageBox(_T("Forkert kodeord"), _T("Fejl"), MB_ICONEXCLAMATION);
    OnCancel();
  } else {
    __super::OnOK();
  }
}

BOOL CPasswordDlg::OnInitDialog() {
  __super::OnInitDialog();
  gotoEditBox(this, IDC_EDITPASSWORD);
  return FALSE;
}
