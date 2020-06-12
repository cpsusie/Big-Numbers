#include "stdafx.h"
#include "TestDirectXDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CTestDirectXApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CTestDirectXApp::CTestDirectXApp() {
}

CTestDirectXApp theApp;

BOOL CTestDirectXApp::InitInstance() {

//DEBUGTRACE;
//  AfxEnableControlContainer();
//DEBUGTRACE;

    CTestDirectXDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK) {
    } else if (nResponse == IDCANCEL) {
    }
    return FALSE;
}
