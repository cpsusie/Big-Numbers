#include "stdafx.h"
#include "PartyMaker.h"
#include "PasswordDlg.h"
#include <md5.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

PasswordDlg::PasswordDlg(CWnd *pParent) : CDialog(PasswordDlg::IDD, pParent) {
    m_password = EMPTYSTRING;
    m_tryCount = 0;
}

void PasswordDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_PASSWORD, m_password);
}

BEGIN_MESSAGE_MAP(PasswordDlg, CDialog)
END_MESSAGE_MAP()

BOOL PasswordDlg::OnInitDialog() {
  GetDlgItem(IDC_PASSWORD)->SetFocus();
  __super::OnInitDialog();
  return false;
}

void PasswordDlg::OnOK() {
  UpdateData();
  if(Options::validatePassword((LPCTSTR)m_password)) {
    __super::OnOK();
  } else {
    Message(_T("Forkert password"));
    m_tryCount++;
    if(m_tryCount == 3) {
      OnCancel();
    }
  }
}
