#include "stdafx.h"
#include "testbin.h"
#include "testbinDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CTestbinApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CTestbinApp::CTestbinApp() {
}

CTestbinApp theApp;

BOOL CTestbinApp::InitInstance() {
    AfxEnableControlContainer();

    CTestbinDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK) {
    } else if (nResponse == IDCANCEL) {
    }

    return FALSE;
}
