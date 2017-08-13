#include "stdafx.h"
#include "Chess.h"
#include "ChessDlg.h"
#include <Timer.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CChessApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CChessApp::CChessApp() {
}

CChessApp theApp;

class OptionsSaver : public TimeoutHandler {
private:
  Options  &m_options;
  Semaphore m_gate;
public:
  OptionsSaver(Options &options) : m_options(options) {
  }
  void handleTimeout(Timer &timer) {
    saveIfDirty();
  }
  void saveIfDirty() {
    m_gate.wait();
//    debugLog(_T("Options.dirty:%s\n"), boolToStr(m_options.isDirty()));
    if(m_options.isDirty()) m_options.save();
    m_gate.signal();
  }
};

BOOL CChessApp::InitInstance() {

#pragma warning(disable : 4101)

  INITCOMMONCONTROLSEX InitCtrls;
  InitCtrls.dwSize = sizeof(InitCtrls);
  // Set this to include all the common control classes you want to use
  // in your application.
  InitCtrls.dwICC = ICC_WIN95_CLASSES;
  InitCommonControlsEx(&InitCtrls);

  __super::InitInstance();

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
      Timer        saveOptionsTimer(1);
      OptionsSaver saveOptions(m_options);
      saveOptionsTimer.startTimer(10000, saveOptions, true);
      dlg.DoModal();
      m_pMainWnd = NULL;
      saveOptionsTimer.stopTimer();
      saveOptions.saveIfDirty();
    }
  } catch(Exception e) {
    showException(e);
  } catch(CSimpleException *e) {
    TCHAR msg[1024];
    e->GetErrorMessage(msg, ARRAYSIZE(msg));
    e->Delete();
    showError(_T("Exception:%s"), msg);
  }

  return FALSE;
}
