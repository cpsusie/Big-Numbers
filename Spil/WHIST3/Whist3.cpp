#include "stdafx.h"
#include "Whist3.h"
#include "GameTypes.h"
#include "Whist3Dialog.h"

BEGIN_MESSAGE_MAP(CWhist3App, CWinApp)
      // NOTE - the ClassWizard will add and remove mapping macros here.
      //    DO NOT EDIT what you see in these blocks of generated code!
  ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWhist3App construction

CWhist3App::CWhist3App() {
}

CWhist3App theApp;

BOOL CWhist3App::InitInstance() {
  AfxEnableControlContainer();

  try {
    CWhist3Dialog dlg;
    m_pMainWnd = &dlg;

    dlg.DoModal();
  } catch(Exception e) {
    log(_T("Exception:%s"),e.what());
  }
  return FALSE;
}
