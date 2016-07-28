#include "stdafx.h"
#include "ParserDemo.h"
#include "ParserDemoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CParserDemoApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CParserDemoApp::CParserDemoApp() {
}

CParserDemoApp theApp;

BOOL CParserDemoApp::InitInstance() {
//  AfxEnableControlContainer();

  SetRegistryKey(_T("JGMData"));

//  LoadStdProfileSettings(10);  // Load standard INI file options (including MRU)

    CParserDemoDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK) {
    } else if (nResponse == IDCANCEL)  {
    }

    return FALSE;
}

