#include "stdafx.h"
#include <MD5.h>
#include "OneOrTwo.h"
#include "PasswordDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CPasswordDlg::CPasswordDlg(CWnd *pParent) : CDialog(CPasswordDlg::IDD, pParent) {
  m_password = EMPTYSTRING;
}

void CPasswordDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITPASSWORD, m_password);
}

static const TCHAR *md5Password = _T("b982898380d9a1d29d9c22998c17df23");

BEGIN_MESSAGE_MAP(CPasswordDlg, CDialog)
END_MESSAGE_MAP()

void CPasswordDlg::OnOK() {
  UpdateData();

  try {
    USES_CONVERSION;
    char *apasswrd = T2A((LPCTSTR)m_password);
    MD5HashCode md5code;
    const String md5 = MD5::getHashCode(md5code, ByteArray((BYTE*)apasswrd, strlen(apasswrd))).toString(false);
    if(md5 == md5Password) {
      __super::OnOK();
    } else {
      OnCancel();
    }
  } catch (Exception e) {
    showException(e);
    OnCancel();
  }
}
