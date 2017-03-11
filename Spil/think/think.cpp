#include "stdafx.h"
#include "think.h"
#include "thinkDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CThinkApp, CWinApp)
  ON_COMMAND(ID_HELP, OnHelp)
END_MESSAGE_MAP()

CThinkApp::CThinkApp() {
}

CThinkApp theApp;

BOOL CThinkApp::InitInstance() {
  AfxEnableControlContainer();

  CThinkDlg dlg;
  m_pMainWnd = &dlg;
  INT_PTR nResponse = dlg.DoModal();
  if (nResponse == IDOK){
  } else if (nResponse == IDCANCEL) {
  }
  return FALSE;
}
