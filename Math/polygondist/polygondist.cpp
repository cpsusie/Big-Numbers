#include "stdafx.h"
#include "polygondistDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CPolygondistApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CPolygondistApp::CPolygondistApp() {
}

CPolygondistApp theApp;

BOOL CPolygondistApp::InitInstance() {
    AfxEnableControlContainer();

    CPolygondistDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK) {
    } else if (nResponse == IDCANCEL) {
    }

    return FALSE;
}
