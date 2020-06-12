#include "stdafx.h"
#include "4paaStribe.h"
#include "4PaaStribeDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CMy4paaStribeApp, CWinApp)
  ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CMy4paaStribeApp::CMy4paaStribeApp() {
}

CMy4paaStribeApp theApp;

BOOL CMy4paaStribeApp::InitInstance() {
  AfxEnableControlContainer();

  C4PaaStribeDlg dlg;
  m_pMainWnd = &dlg;
  INT_PTR nResponse = dlg.DoModal();
  if (nResponse == IDOK) {
  } else if (nResponse == IDCANCEL) {
  }
  // Since the dialog has been closed, return FALSE so that we exit the
  //  application, rather than start the application's message pump.
  return FALSE;
}
