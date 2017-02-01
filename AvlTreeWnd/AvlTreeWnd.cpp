#include "stdafx.h"
#include "AvlTreeWnd.h"
#include "AvlTreeWndDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CAvlTreeWndApp, CWinApp)
  ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CAvlTreeWndApp::CAvlTreeWndApp() {
}

CAvlTreeWndApp theApp;

BOOL CAvlTreeWndApp::InitInstance() {

    CAvlTreeWndDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK) {
    } else if (nResponse == IDCANCEL) {
    }

    // Since the dialog has been closed, return FALSE so that we exit the
    //  application, rather than start the application's message pump.
    return FALSE;
}
