#include "stdafx.h"
#include "MazeApp.h"
#include "mazeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CMazeApp, CWinApp)
  ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CMazeApp::CMazeApp() {
}

CMazeApp theApp;

BOOL CMazeApp::InitInstance() {
  AfxEnableControlContainer();

  CMazeDlg dlg;
  m_pMainWnd = &dlg;
  INT_PTR nResponse = dlg.DoModal();
  if (nResponse == IDOK) {
  } else if(nResponse == IDCANCEL) {
  }

  return FALSE;
}
