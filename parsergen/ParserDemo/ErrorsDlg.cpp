#include "stdafx.h"
#include "ParserDemo.h"
#include "ErrorsDlg.h"

#ifdef _DEBUG
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
  ON_COMMAND(ID_GOTOERRORCASCADECOUNT, OnGotoErrorCascadeCount)
  ON_COMMAND(ID_GOTOMAXERRORCOUNT    , OnGotoMaxErrorCount    )
END_MESSAGE_MAP()

void ErrorsDlg::OnOK() {
  UpdateData();
  __super::OnOK();
}

BOOL ErrorsDlg::OnInitDialog() {
  __super::OnInitDialog();
  m_accelTable = LoadAccelerators(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_ACCELERATORERRORS));
  gotoEditBox(this, IDC_EDITMAXERRORCOUNT);
  return false;
}

void ErrorsDlg::OnGotoErrorCascadeCount() {
  gotoEditBox(this, IDC_EDITCASCADECOUNT);
}

void ErrorsDlg::OnGotoMaxErrorCount() {
  gotoEditBox(this, IDC_EDITMAXERRORCOUNT);
}

BOOL ErrorsDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}
