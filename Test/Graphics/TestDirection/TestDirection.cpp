
#include "stdafx.h"
#include "TestDirection.h"
#include "TestDirectionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CTestDirectionApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CTestDirectionApp::CTestDirectionApp()
{
}

CTestDirectionApp theApp;

BOOL CTestDirectionApp::InitInstance() {
    AfxEnableControlContainer();

    CTestDirectionDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK) {
    } else if (nResponse == IDCANCEL) {
    }
    return FALSE;
}
