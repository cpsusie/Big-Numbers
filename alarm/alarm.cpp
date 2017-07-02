#include "stdafx.h"
#include "AlarmDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CAlarmApp, CWinApp)
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
