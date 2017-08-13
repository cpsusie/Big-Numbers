#include "stdafx.h"
#include "TestEdgeDetectionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CTestEdgeDetectionApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CTestEdgeDetectionApp::CTestEdgeDetectionApp() {
}

CTestEdgeDetectionApp theApp;

BOOL CTestEdgeDetectionApp::InitInstance() {
    AfxEnableControlContainer();

    try {
      CTestEdgeDetectionDlg dlg;
      m_pMainWnd = &dlg;
      INT_PTR nResponse = dlg.DoModal();
      if(nResponse == IDOK) {
      } else if(nResponse == IDCANCEL) {
      }
    } catch(Exception e) {
      showException(e);
    }
    return FALSE;
}
