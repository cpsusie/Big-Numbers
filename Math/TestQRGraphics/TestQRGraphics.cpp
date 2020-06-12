#include "stdafx.h"
#include "TestQRGraphicsDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CTestQRGraphicsApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CTestQRGraphicsApp::CTestQRGraphicsApp() {
}

CTestQRGraphicsApp theApp;

BOOL CTestQRGraphicsApp::InitInstance() {
    AfxEnableControlContainer();

    CTestQRGraphicsDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK) {
    } else if (nResponse == IDCANCEL) {
    }
    return FALSE;
}
