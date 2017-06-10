#include "stdafx.h"
#include "AlarmDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CAlarmApp, CWinApp)
  //{{AFX_MSG_MAP(CAlarmApp)
  //}}AFX_MSG
  ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CAlarmApp::CAlarmApp() {
}

CAlarmApp theApp;

BOOL CAlarmApp::InitInstance() {
  AfxEnableControlContainer();

  CAlarmDlg dlg;
  m_pMainWnd = &dlg;
  INT_PTR nResponse = dlg.DoModal();
  if(nResponse == IDOK) {
  } else if (nResponse == IDCANCEL) {
  }
  return FALSE;
}
