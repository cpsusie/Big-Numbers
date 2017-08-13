#include "stdafx.h"
#include "TestLexStreamDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CTestLexStreamApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CTestLexStreamApp::CTestLexStreamApp() {
}

CTestLexStreamApp theApp;

BOOL CTestLexStreamApp::InitInstance() {
    AfxEnableControlContainer();

    try {
      CTestLexStreamDlg dlg;
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
