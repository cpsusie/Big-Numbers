#include "stdafx.h"
#include "GetResourcesDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CGetresourcesApp, CWinApp)
    ON_COMMAND(ID_HELP, OnHelp)
END_MESSAGE_MAP()

CGetresourcesApp::CGetresourcesApp() {
}

CGetresourcesApp theApp;

BOOL CGetresourcesApp::InitInstance() {
    AfxEnableControlContainer();

    CGetresourcesDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if(nResponse == IDOK) {
    } else if(nResponse == IDCANCEL) {
    }
    return FALSE;
}
