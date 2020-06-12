#include "stdafx.h"
#include "CalculatorDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CCalculatorApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CCalculatorApp::CCalculatorApp() {
}

CCalculatorApp theApp;

BOOL CCalculatorApp::InitInstance() {
    AfxEnableControlContainer();

    CCalculatorDlg dlg;
    m_pMainWnd = &dlg;
    dlg.m_hInstance = m_hInstance;
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK) {
    } else if (nResponse == IDCANCEL) {
    }

    return FALSE;
}
