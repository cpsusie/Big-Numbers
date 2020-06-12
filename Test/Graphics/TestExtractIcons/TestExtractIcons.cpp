#include "stdafx.h"
#include "TestExtractIconsDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CTestExtractIconsApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CTestExtractIconsApp::CTestExtractIconsApp() {
}

CTestExtractIconsApp theApp;

BOOL CTestExtractIconsApp::InitInstance() {
    AfxEnableControlContainer();

    CTestExtractIconsDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if(nResponse == IDOK)   {
    } else if (nResponse == IDCANCEL) {
    }

    return FALSE;
}
