#include "stdafx.h"
#include "SortDemo.h"
#include "SortDemoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CSortDemoApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CSortDemoApp::CSortDemoApp() {
}

CSortDemoApp theApp;


BOOL CSortDemoApp::InitInstance() {
  AfxEnableControlContainer();

  CSortDemoDlg dlg;
  m_pMainWnd = &dlg;
  INT_PTR nResponse = dlg.DoModal();
  if (nResponse == IDOK) {
  } else if (nResponse == IDCANCEL) {
  }

  return FALSE;
}
