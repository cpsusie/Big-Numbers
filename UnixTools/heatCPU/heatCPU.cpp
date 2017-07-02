#include "stdafx.h"
#include "heatCPUDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CHeatCPUApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CHeatCPUApp::CHeatCPUApp() {
}

CHeatCPUApp theApp;

BOOL CHeatCPUApp::InitInstance() {
    AfxEnableControlContainer();

    try {
      CHeatCPUDlg dlg;
      m_pMainWnd = &dlg;
      dlg.DoModal();
    } catch(Exception e) {
      debugLog(_T("Exception:%s\n"), e.what());
    }

    return FALSE;
}
