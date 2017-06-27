#include "stdafx.h"
#include "TestMediaPlayerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CTestMediaPlayerApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CTestMediaPlayerApp::CTestMediaPlayerApp() {
}

CTestMediaPlayerApp theApp;

BOOL CTestMediaPlayerApp::InitInstance() {
    AfxEnableControlContainer();

    CTestMediaPlayerDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if(nResponse == IDOK) {
    } else if(nResponse == IDCANCEL) {
    }

    return FALSE;
}
