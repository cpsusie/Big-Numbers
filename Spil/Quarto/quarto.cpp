#include "stdafx.h"
#include "QuartoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CQuartoApp, CWinApp)
END_MESSAGE_MAP()

CQuartoApp::CQuartoApp() {
}

CQuartoApp theApp;

BOOL CQuartoApp::InitInstance() {
  AfxEnableControlContainer();

  try {
	  CQuartoDlg dlg;
	  m_pMainWnd = &dlg;
	  dlg.DoModal();
  } catch(Exception e) {
    showException(e);
  }
  return FALSE;
}
