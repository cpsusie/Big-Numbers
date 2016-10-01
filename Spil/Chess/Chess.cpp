#include "stdafx.h"
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

/*
  GameKey key;
  int size = sizeof(Game);
  size = sizeof(key);
  size = sizeof(key.d);
  size = sizeof(key.m_u);
  size = sizeof(AttackInfo);
  size = sizeof(AttackDirectionInfo);
  size = sizeof(FieldAttacks);
  size = sizeof(CChessDlg);
  size = sizeof(PlayerState);
  size = sizeof(PlayerStateToPush);
*/
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
