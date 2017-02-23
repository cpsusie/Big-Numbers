#include "stdafx.h"
#include "MakeAvi.h"
#include "MakeAviDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CMakeAviApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CMakeAviApp::CMakeAviApp()
{
}

CMakeAviApp theApp;

BOOL CMakeAviApp::InitInstance() {
  AfxEnableControlContainer();

  CMakeAviDlg dlg;
  m_pMainWnd = &dlg;
  INT_PTR nResponse = dlg.DoModal();
  if(nResponse == IDOK) {
  }
  else if(nResponse == IDCANCEL) {
  }

  return FALSE;
}
