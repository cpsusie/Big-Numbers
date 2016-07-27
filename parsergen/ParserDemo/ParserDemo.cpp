#include "stdafx.h"
#include "ParserDemo.h"
#include "ParserDemoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CParserDemoApp, CWinApp)
    //{{AFX_MSG_MAP(CParserDemoApp)
    //}}AFX_MSG
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CParserDemoApp::CParserDemoApp() {
}

CParserDemoApp theApp;

BOOL CParserDemoApp::InitInstance() {
//  AfxEnableControlContainer();

#ifdef _AFXDLL
    Enable3dControls();         // Call this when using MFC in a shared DLL
#else
    Enable3dControlsStatic();   // Call this when linking to MFC statically
#endif

  SetRegistryKey(_T("JGMData"));

//  LoadStdProfileSettings(10);  // Load standard INI file options (including MRU)

    CParserDemoDlg dlg;
    m_pMainWnd = &dlg;
    int nResponse = dlg.DoModal();
    if (nResponse == IDOK)
    {
        // TODO: Place code here to handle when the dialog is
        //  dismissed with OK
    }
    else if (nResponse == IDCANCEL)
    {
        // TODO: Place code here to handle when the dialog is
        //  dismissed with Cancel
    }

    // Since the dialog has been closed, return FALSE so that we exit the
    //  application, rather than start the application's message pump.
    return FALSE;
}

