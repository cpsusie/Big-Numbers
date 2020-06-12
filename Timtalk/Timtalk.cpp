#include "stdafx.h"
#include "TimtalkDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CTimtalkApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CTimtalkApp::CTimtalkApp() {
}

CTimtalkApp theApp;

BOOL CTimtalkApp::InitInstance() {
    AfxEnableControlContainer();

    CTimtalkDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK) {
    } else if (nResponse == IDCANCEL) {
    }
    return FALSE;
}
