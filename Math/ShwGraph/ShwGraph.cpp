#include "stdafx.h"
#include "ShwGraphDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CShwgraphApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CShwgraphApp::CShwgraphApp() {
}

CShwgraphApp theApp;

BOOL CShwgraphApp::InitInstance() {
    AfxEnableControlContainer();

    // Standard initialization
    // If you are not using these features and wish to reduce the size
    //  of your final executable, you should remove from the following
    //  the specific initialization routines you do not need.

    CShwGraphDlg dlg;
    m_pMainWnd = &dlg;
    try {
      intptr_t nResponse = dlg.DoModal();
      if(nResponse == IDOK) {
      } else if(nResponse == IDCANCEL) {
      }
    } catch(Exception e) {
      showException(e);
    }

    // Since the dialog has been closed, return FALSE so that we exit the
    // application, rather than start the application's message pump.
    return FALSE;
}
