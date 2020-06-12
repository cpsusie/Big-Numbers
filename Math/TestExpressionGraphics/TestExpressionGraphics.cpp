#include "stdafx.h"
#include "TestExpressionGraphicsDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CTestExpressionGraphicsApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CTestExpressionGraphicsApp::CTestExpressionGraphicsApp() {
}

CTestExpressionGraphicsApp theApp;

BOOL CTestExpressionGraphicsApp::InitInstance() {
    AfxEnableControlContainer();

    // Standard initialization

    try {
      CTestExpressionGraphicsDlg dlg;
      m_pMainWnd = &dlg;
      INT_PTR nResponse = dlg.DoModal();
      if (nResponse == IDOK) {
      } else if (nResponse == IDCANCEL) {
      }
    } catch (Exception e) {
      showException(e);
    }
    return FALSE;
}
