#include "stdafx.h"
#include "draw3d.h"
#include "draw3dDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CDraw3dApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CDraw3dApp::CDraw3dApp() {
}

CDraw3dApp theApp;

BOOL CDraw3dApp::InitInstance() {
  AfxEnableControlContainer();

  CDraw3dDlg dlg;
  m_pMainWnd = &dlg;
  INT_PTR nResponse = dlg.DoModal();
  if (nResponse == IDOK) {
  } else if (nResponse == IDCANCEL) {
  }

  return FALSE;
}
