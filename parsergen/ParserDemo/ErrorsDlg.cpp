#include "stdafx.h"
#include "ErrorsDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

ErrorsDlg::ErrorsDlg(int maxErrorCount, int cascadeCount, CWnd *pParent) : CDialog(ErrorsDlg::IDD, pParent) {
  m_maxErrorCount = maxErrorCount;
  m_cascadeCount  = cascadeCount;
}

void ErrorsDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITCASCADECOUNT , m_cascadeCount );
  DDX_Text(pDX, IDC_EDITMAXERRORCOUNT, m_maxErrorCount);
}

BEGIN_MESSAGE_MAP(ErrorsDlg, CDialog)
END_MESSAGE_MAP()

void ErrorsDlg::OnOK() {
  UpdateData();
  __super::OnOK();
}

BOOL ErrorsDlg::OnInitDialog() {
  __super::OnInitDialog();
  gotoEditBox(this, IDC_EDITMAXERRORCOUNT);
  return false;
}
