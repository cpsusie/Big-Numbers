#include "stdafx.h"
#include "TestWebCam.h"
#include "TestWebCamDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CTestWebCamApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CTestWebCamApp::CTestWebCamApp() {
}

CTestWebCamApp theApp;

BOOL CTestWebCamApp::InitInstance() {
	AfxEnableControlContainer();

  try {
	  CTestWebCamDlg dlg;
	  m_pMainWnd = &dlg;
	  INT_PTR nResponse = dlg.DoModal();
	  if (nResponse == IDOK) {
	  } else if (nResponse == IDCANCEL)	{
	  }
  } catch(Exception e) {
    showException(e);
  }
	return FALSE;
}
