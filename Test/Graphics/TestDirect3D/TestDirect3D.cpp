#include "stdafx.h"
#include "TestDirect3D.h"
#include "TestDirect3DDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CTestDirect3DApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CTestDirect3DApp::CTestDirect3DApp()
{
}

CTestDirect3DApp theApp;

BOOL CTestDirect3DApp::InitInstance() {
    AfxEnableControlContainer();

    CTestDirect3DDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK) {
    } else if (nResponse == IDCANCEL) {
    }
    return FALSE;
}
