#include "stdafx.h"
#include "PartyMaker.h"
#include "PasswordDlg.h"
#include "ChangePasswordDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CChangePasswordDlg::CChangePasswordDlg(CWnd *pParent) : CDialog(CChangePasswordDlg::IDD, pParent) {
    m_oldPassword  = EMPTYSTRING;
    m_newPassword  = EMPTYSTRING;
    m_newPassword2 = EMPTYSTRING;
    m_tryCount = 0;
}

BOOL CChangePasswordDlg::OnInitDialog() {
  GetDlgItem(IDC_OLDPASSWORDEDIT)->SetFocus();
  CDialog::OnInitDialog();
  return false;
}

void CChangePasswordDlg::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_OLDPASSWORDEDIT , m_oldPassword );
    DDX_Text(pDX, IDC_NEWPASSWORDEDIT , m_newPassword );
    DDX_Text(pDX, IDC_NEWPASSWORDEDIT2, m_newPassword2);
}

BEGIN_MESSAGE_MAP(CChangePasswordDlg, CDialog)
END_MESSAGE_MAP()

void CChangePasswordDlg::OnOK() {
  UpdateData(true);
  if(m_newPassword != m_newPassword2) {
    Message(_T("De indtastede passwords er ikke ens"));
    GetDlgItem(IDC_NEWPASSWORDEDIT)->SetFocus();
  } else {
    try {
      Options::setPassword((LPCTSTR)m_oldPassword,(LPCTSTR)m_newPassword);
      CDialog::OnOK();
    } catch(Exception e) {
      Message(e.what());
      m_tryCount++;
      if(m_tryCount == 3) {
        OnCancel();
      } else {
        GetDlgItem(IDC_OLDPASSWORDEDIT)->SetFocus();
      }
    }
  }
}
