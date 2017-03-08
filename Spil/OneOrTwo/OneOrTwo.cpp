#include "stdafx.h"
#include "OneOrTwo.h"
#include "OneOrTwoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(COneOrTwoApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

COneOrTwoApp::COneOrTwoApp() {
}

COneOrTwoApp theApp;

BOOL COneOrTwoApp::InitInstance() {
    AfxEnableControlContainer();

    COneOrTwoDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK) {
    } else if (nResponse == IDCANCEL) {
    }

    return FALSE;
}
