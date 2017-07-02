#include "stdafx.h"
#include "asciiwDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CAsciiwApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CAsciiwApp::CAsciiwApp() {
}

CAsciiwApp theApp;

BOOL CAsciiwApp::InitInstance() {
    AfxEnableControlContainer();

    CAsciiwDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK) {
    } else if (nResponse == IDCANCEL) {
    }
    return FALSE;
}
