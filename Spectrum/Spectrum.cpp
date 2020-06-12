#include "stdafx.h"
#include "Spectrum.h"
#include "SpectrumDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CSpectrumApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CSpectrumApp::CSpectrumApp() {
}

CSpectrumApp theApp;

BOOL CSpectrumApp::InitInstance() {
	AfxEnableControlContainer();

	CSpectrumDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if(nResponse == IDOK) {
  } else if (nResponse == IDCANCEL) {
	}

	return FALSE;
}
