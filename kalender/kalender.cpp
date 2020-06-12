#include "stdafx.h"
#include "kalenderDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CKalenderApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CKalenderApp::CKalenderApp() {
}

CKalenderApp theApp;

BOOL CKalenderApp::InitInstance() {
    AfxEnableControlContainer();

    CKalenderDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK) {
    } else if (nResponse == IDCANCEL) {
    }
    return FALSE;
}
