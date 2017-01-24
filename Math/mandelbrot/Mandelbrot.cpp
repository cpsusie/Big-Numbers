#include "stdafx.h"
#include "Mandelbrot.h"
#include "MandelbrotDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CMandelbrotApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CMandelbrotApp::CMandelbrotApp() {
}

CMandelbrotApp theApp;

BOOL CMandelbrotApp::InitInstance() {
    AfxEnableControlContainer();

    try {
      CMandelbrotDlg dlg;
      m_pMainWnd = &dlg;
      INT_PTR nResponse = dlg.DoModal();
      if(nResponse == IDOK) {
      } else if (nResponse == IDCANCEL) {
      }
    } catch(Exception e) {
      AfxMessageBox(e.what(),MB_ICONSTOP);
    }

    return FALSE;
}
