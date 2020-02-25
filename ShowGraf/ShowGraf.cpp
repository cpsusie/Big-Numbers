#include "stdafx.h"
#include <afxadv.h>
#include "MainFrm.h"
#include "ShowGrafDoc.h"
#include "ShowGrafView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CShowGrafApp, CWinApp)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
    ON_COMMAND(ID_FILE_NEW, __super::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, __super::OnFileOpen)
END_MESSAGE_MAP()

CShowGrafApp::CShowGrafApp() {
}

CShowGrafApp theApp;

BOOL CShowGrafApp::InitInstance() {
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    // Set this to include all the common control classes you want to use
    // in your application.
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    __super::InitInstance();

    EnableTaskbarInteraction(FALSE);

    SetRegistryKey(_T("JGMData"));

    LoadStdProfileSettings(16);

    CSingleDocTemplate *pDocTemplate = new CSingleDocTemplate(IDR_MAINFRAME
                                                             ,RUNTIME_CLASS(CShowGrafDoc)
                                                             ,RUNTIME_CLASS(CMainFrame)
                                                             ,RUNTIME_CLASS(CShowGrafView));
    AddDocTemplate(pDocTemplate);

    CCommandLineInfo cmdInfo;
    if(!ProcessShellCommand(cmdInfo)) {
      return FALSE;
    }
    m_pMainWnd->ShowWindow(SW_SHOW);
    m_pMainWnd->UpdateWindow();
    CShowGrafDoc *doc = getMainWindow()->getDoc();
    doc->addArgvGraphs();
    if(doc->isOK()) {
      getMainWindow()->activateInitialOptions();
    } else {
      showWarning(doc->getErrorMessage());
      PostMessage(*m_pMainWnd, WM_QUIT,0,0);
    }
    return TRUE;
}

String CShowGrafApp::getRecentFile(int index) {
  CRecentFileList &list = *m_pRecentFileList;
  if(index >= list.GetSize()) {
    return EMPTYSTRING;
  }
  CString name = list[index];
  return (LPCTSTR)name;
}

class CAboutDlg : public CDialog {
public:
  enum { IDD = IDD_ABOUTBOX };
  CAboutDlg() : CDialog(IDD) {
  }
protected:
  DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

void CShowGrafApp::OnAppAbout() {
  CAboutDlg().DoModal();
}
