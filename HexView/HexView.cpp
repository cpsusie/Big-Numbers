#include "stdafx.h"
#include <afxadv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CHexViewApp, CWinApp)
  ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
  // Standard file based document commands
  ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
  // Standard print setup command
END_MESSAGE_MAP()

CHexViewApp::CHexViewApp() {
  SetAppID(_T("HexView version 2.001"));
}

CHexViewApp theApp;

BOOL CHexViewApp::InitInstance() {

    // InitCommonControlsEx() is required on Windows XP if an application
    // manifest specifies use of ComCtl32.dll version 6 or later to enable
    // visual styles.  Otherwise, any window creation will fail.
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    // Set this to include all the common control classes you want to use
    // in your application.
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    __super::InitInstance();

    // Initialize OLE libraries
    if (!AfxOleInit()) {
        AfxMessageBox(IDP_OLE_INIT_FAILED);
        return FALSE;
    }

    AfxEnableControlContainer();

    EnableTaskbarInteraction(FALSE);

  SetRegistryKey(_T("JGMData"));
  LoadStdProfileSettings(_AFX_MRU_MAX_COUNT);

  CSingleDocTemplate *pDocTemplate = new CSingleDocTemplate(IDR_MAINFRAME
                                                           ,RUNTIME_CLASS(CHexViewDoc)
                                                           ,RUNTIME_CLASS(CMainFrame)       // main SDI frame window
                                                           ,RUNTIME_CLASS(CHexViewView));
  AddDocTemplate(pDocTemplate);

  CCommandLineInfo cmdInfo;
  ParseCommandLine(cmdInfo);

  if(!ProcessShellCommand(cmdInfo)) {
    return FALSE;
  }

  m_pMainWnd->ShowWindow(SW_SHOW);
  m_pMainWnd->UpdateWindow();

  TCHAR **argv = __targv;
  *argv++;
  TCHAR *name = *argv++;
  if(name) {
    if(((CMainFrame*)m_pMainWnd)->newFile(name, true)) {
      addToRecentFileList(name);
    }
  }

  return TRUE;
}

void CHexViewApp::addToRecentFileList(const String &name) {
  try {
    CRecentFileList &list = *m_pRecentFileList;
    list.Add(name.cstr());
  } catch (CException * e) {
    TCHAR strCause[1000];
    e->GetErrorMessage(strCause, ARRAYSIZE(strCause));
//    debugLog(_T("Exception in %s: %s.  -- Ignoring\n"), __TFUNCTION__, strCause);
    e->Delete();
  }
}

String CHexViewApp::getRecentFile(int index) {
  CRecentFileList &list = *m_pRecentFileList;
  if(index >= list.GetSize()) {
    return EMPTYSTRING;
  } else {
    return (LPCTSTR)list[index];
  }
}

void CHexViewApp::removeFromRecentFiles(int index) {
  CRecentFileList &list = *m_pRecentFileList;
  if(index >= list.GetSize()) {
    return;
  }
  list.Remove(index);
}

class CAboutDlg : public CDialog {
public:
  enum { IDD = IDD_ABOUTBOX };
  CAboutDlg() : CDialog(IDD) {
  }

  DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

void CHexViewApp::OnAppAbout() {
  CAboutDlg().DoModal();
}
