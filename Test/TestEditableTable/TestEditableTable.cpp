#include "stdafx.h"
#include "TestEditableTable.h"
#include "TestEditableTableDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CTestEditableTableApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CTestEditableTableApp::CTestEditableTableApp() {
}

CTestEditableTableApp theApp;

BOOL CTestEditableTableApp::InitInstance() {
    AfxEnableControlContainer();

    CTestEditableTableDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if(nResponse == IDOK) {
    } else if (nResponse == IDCANCEL) {
    }

    return FALSE;
}
