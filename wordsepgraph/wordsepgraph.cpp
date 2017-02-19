#include "stdafx.h"
#include "wordsepgraph.h"
#include "wordsepgraphDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CWordsepgraphApp, CWinApp)
  ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CWordsepgraphApp::CWordsepgraphApp() {
}

CWordsepgraphApp theApp;

BOOL CWordsepgraphApp::InitInstance() {
  AfxEnableControlContainer();

  CWordsepgraphDlg dlg;
  m_pMainWnd = &dlg;
  INT_PTR nResponse = dlg.DoModal();
  if(nResponse == IDOK) {
  } else if (nResponse == IDCANCEL) {
  }

  return FALSE;
}
