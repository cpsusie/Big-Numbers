#include "stdafx.h"
#include "TestLexStream.h"
#include "TestLexStreamDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CTestLexStreamApp, CWinApp)
    //{{AFX_MSG_MAP(CTestLexStreamApp)
    //}}AFX_MSG
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CTestLexStreamApp::CTestLexStreamApp() {
}

CTestLexStreamApp theApp;

BOOL CTestLexStreamApp::InitInstance() {
    AfxEnableControlContainer();

#ifdef _AFXDLL
    Enable3dControls();
#else
    Enable3dControlsStatic();
#endif

    try {
      CTestLexStreamDlg dlg;
      m_pMainWnd = &dlg;
      int nResponse = dlg.DoModal();
      if (nResponse == IDOK) {
      } else if (nResponse == IDCANCEL) {
      }
    } catch(Exception e) {
      AfxMessageBox(e.what(), MB_ICONWARNING);
    }
    return FALSE;
}
