#include "stdafx.h"
#include "GetColorDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CGetcolorApp, CWinApp)
  ON_COMMAND(ID_HELP, OnHelp)
END_MESSAGE_MAP()

CGetcolorApp::CGetcolorApp() {
}

CGetcolorApp theApp;

BOOL CGetcolorApp::InitInstance() {
  AfxEnableControlContainer();

  CGetcolorDlg dlg;
  m_pMainWnd = &dlg;
  INT_PTR nResponse = dlg.DoModal();
  if (nResponse == IDOK) {
  } else if (nResponse == IDCANCEL) {
  }
  return FALSE;
}
