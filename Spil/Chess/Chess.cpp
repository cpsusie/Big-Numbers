#include "stdafx.h"
#include "Chess.h"
#include "ChessDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CChessApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CChessApp::CChessApp() {
}

CChessApp theApp;

BOOL CChessApp::InitInstance() {

#pragma warning(disable : 4101)

  INITCOMMONCONTROLSEX InitCtrls;
  InitCtrls.dwSize = sizeof(InitCtrls);
  // Set this to include all the common control classes you want to use
  // in your application.
  InitCtrls.dwICC = ICC_WIN95_CLASSES;
  InitCommonControlsEx(&InitCtrls);

  CWinApp::InitInstance();

  AfxEnableControlContainer();

  // Activate "Windows Native" visual manager for enabling themes in MFC controls
  CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

  TCHAR **argv = __targv + 1;

  try {
    String argument;
    if(*argv) {
      argument = *argv;
    }
    if(argument == _T("-install")) {
      createFileAssociation();
    } else if(argument == _T("-R")) {
      redirectDebugLog(true);
      RemoteEndGameSubTablebase::remoteService(argv);
    } else {
      redirectDebugLog();
      setCurrentLanguage(m_options.getLangID());
      CChessDlg dlg(argument);
      m_pMainWnd = &dlg;
      dlg.DoModal();
      m_pMainWnd = NULL;
    }
  } catch(Exception e) {
    AfxMessageBox(format(_T("Exception:%s"), e.what()).cstr(), MB_ICONSTOP);
  } catch(CSimpleException *e) {
    TCHAR msg[1024];
    e->GetErrorMessage(msg, ARRAYSIZE(msg));
    e->Delete();
    AfxMessageBox(format(_T("Exception:%s"), msg).cstr(), MB_ICONSTOP);
  }

  return FALSE;
}
