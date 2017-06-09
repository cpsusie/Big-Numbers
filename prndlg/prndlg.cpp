#include "stdafx.h"
#include "prndlg.h"
#include "prndlgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CPrndlgApp, CWinApp)
  ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CPrndlgApp::CPrndlgApp() {
}

CPrndlgApp theApp;

BOOL CPrndlgApp::InitInstance() {
  AfxEnableControlContainer();

  CPrndlgDlg dlg;
  m_pMainWnd = &dlg;
  INT_PTR nResponse = dlg.DoModal();
  if (nResponse == IDOK) {
  } else if (nResponse == IDCANCEL) {
  }
  return FALSE;
}
