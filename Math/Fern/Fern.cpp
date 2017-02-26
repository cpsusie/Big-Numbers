#include "stdafx.h"
#include "Fern.h"
#include "FernDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CFernApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CFernApp::CFernApp() {
}

CFernApp theApp;

BOOL CFernApp::InitInstance() {
    AfxEnableControlContainer();

    CFernDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK) {
    } else if (nResponse == IDCANCEL) {
    }

    return FALSE;
}
