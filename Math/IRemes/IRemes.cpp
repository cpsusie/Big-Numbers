#include "stdafx.h"
#include "IRemes.h"
#include "IRemesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CIRemesApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CIRemesApp::CIRemesApp() {
}

CIRemesApp theApp;

BOOL CIRemesApp::InitInstance() {
    AfxEnableControlContainer();

    CIRemesDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if(nResponse == IDOK) {
    } else if(nResponse == IDCANCEL) {
    }

    return FALSE;
}
