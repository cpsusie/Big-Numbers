#include "stdafx.h"
#include "WinDiff.h"
#include "EnterFormatNameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CGetFormatNameDlg::CGetFormatNameDlg(const String &name, CWnd *pParent) : CDialog(CGetFormatNameDlg::IDD, pParent) {
  m_name = name.cstr();
}

void CGetFormatNameDlg::DoDataExchange(CDataExchange* pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITNAME, m_name);
}

BEGIN_MESSAGE_MAP(CGetFormatNameDlg, CDialog)
END_MESSAGE_MAP()

void CGetFormatNameDlg::OnOK() {
  UpdateData(); 
  __super::OnOK();
}

BOOL CGetFormatNameDlg::OnInitDialog() {
  __super::OnInitDialog();
  gotoEditBox(this, IDC_EDITNAME);
  return false;
}
