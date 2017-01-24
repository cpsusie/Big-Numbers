#include "stdafx.h"
#include "D3FunctionPlotter.h"
#include "D3FunctionPlotterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CD3FunctionPlotterApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CD3FunctionPlotterApp::CD3FunctionPlotterApp() {
}

CD3FunctionPlotterApp theApp;

BOOL CD3FunctionPlotterApp::InitInstance() {

  AfxEnableControlContainer();

  try {
    CD3FunctionPlotterDlg dlg;
    m_pMainWnd = &dlg;
    intptr_t nResponse = dlg.DoModal();
    if (nResponse == IDOK) {
    } else if (nResponse == IDCANCEL) {
    }
  } catch(Exception e) {
    AfxMessageBox(e.what(), MB_ICONSTOP);
  }
  return FALSE;
}
