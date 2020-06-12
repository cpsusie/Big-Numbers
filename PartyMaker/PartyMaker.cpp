#include "stdafx.h"
#include "PartyMaker.h"
#include "PartyMakerDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CPartyMakerApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CPartyMakerApp::CPartyMakerApp() {
}

CPartyMakerApp theApp;

BOOL CPartyMakerApp::InitInstance() {
  AfxEnableControlContainer();

  CPartyMakerDlg dlg;
  m_pMainWnd = &dlg;
  INT_PTR nResponse = dlg.DoModal();
  if (nResponse == IDOK) {
  } else if (nResponse == IDCANCEL) {
  }

  return FALSE;
}

