#include "stdafx.h"
#include "TestBin.h"
#include "TestInt64Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CTestBinApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CTestBinApp::CTestBinApp() {
}

CTestBinApp theApp;

BOOL CTestBinApp::InitInstance() {
  AfxEnableControlContainer();

  CTestInt64Dlg dlg;
  m_pMainWnd = &dlg;
  INT_PTR nResponse = dlg.DoModal();
  if(nResponse == IDOK) {
  } else if (nResponse == IDCANCEL) {
  }

  return FALSE;
}
