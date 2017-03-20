#include "stdafx.h"
#include "TestAVIFile.h"
#include "TestAVIFileDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CTestAVIFileApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CTestAVIFileApp::CTestAVIFileApp() {
}

CTestAVIFileApp theApp;

BOOL CTestAVIFileApp::InitInstance() {
  AfxEnableControlContainer();

  // Standard initialization

  CTestAVIFileDlg dlg;
  m_pMainWnd = &dlg;
  INT_PTR nResponse = dlg.DoModal();
  if (nResponse == IDOK) {
  } else if (nResponse == IDCANCEL) {
  }

  return FALSE;
}
