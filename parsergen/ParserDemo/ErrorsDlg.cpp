#include "stdafx.h"
#include <WinTools.h>
#include "ParserDemo.h"
#include "ErrorsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

ErrorsDlg::ErrorsDlg(int maxErrorCount, int cascadeCount, CWnd* pParent) : CDialog(ErrorsDlg::IDD, pParent) {
  //{{AFX_DATA_INIT(ErrorsDlg)
  m_maxErrorCount = maxErrorCount;
  m_cascadeCount  = cascadeCount;
  //}}AFX_DATA_INIT
}

void ErrorsDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(ErrorsDlg)
  DDX_Text(pDX, IDC_EDITCASCADECOUNT , m_cascadeCount );
  DDX_Text(pDX, IDC_EDITMAXERRORCOUNT, m_maxErrorCount);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(ErrorsDlg, CDialog)
  //{{AFX_MSG_MAP(ErrorsDlg)
  ON_COMMAND(ID_GOTOERRORCASCADECOUNT, OnGotoErrorCascadeCount)
  ON_COMMAND(ID_GOTOMAXERRORCOUNT    , OnGotoMaxErrorCount    )
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void ErrorsDlg::OnOK() {
  UpdateData();
  CDialog::OnOK();
}

BOOL ErrorsDlg::OnInitDialog() {
  CDialog::OnInitDialog();
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

BOOL ErrorsDlg::PreTranslateMessage(MSG* pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    return true;
  }
  return CDialog::PreTranslateMessage(pMsg);
}
