#include "stdafx.h"
#include "ShowDirAndArg.h"
#include "ShowDirAndArgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CShowDirAndArgApp, CWinApp)
    ON_COMMAND(ID_HELP, OnHelp)
END_MESSAGE_MAP()

CShowDirAndArgApp::CShowDirAndArgApp() {
}

CShowDirAndArgApp theApp;

BOOL CShowDirAndArgApp::InitInstance() {
  CShowDirAndArgDlg dlg;
  m_pMainWnd = &dlg;
  INT_PTR nResponse = dlg.DoModal();
  if (nResponse == IDOK) {
    exit(0);
  } else if (nResponse == IDCANCEL) {
    exit(-1);
  }
  return FALSE;
}
