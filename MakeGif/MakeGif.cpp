#include "stdafx.h"
#include <afxadv.h>

#include "MainFrm.h"
#include "MakeGifDoc.h"
#include "MakeGifView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CMakeGifApp, CWinApp)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
END_MESSAGE_MAP()

CMakeGifApp::CMakeGifApp() {
}

CMakeGifApp theApp;

BOOL CMakeGifApp::InitInstance() {
  INITCOMMONCONTROLSEX InitCtrls;
  InitCtrls.dwSize = sizeof(InitCtrls);
  // Set this to include all the common control classes you want to use
  // in your application.
  InitCtrls.dwICC = ICC_WIN95_CLASSES;
  InitCommonControlsEx(&InitCtrls);

  CWinApp::InitInstance();

  EnableTaskbarInteraction(FALSE);

  SetRegistryKey(_T("JGMData"));

  LoadStdProfileSettings(16);

  CSingleDocTemplate* pDocTemplate;
  pDocTemplate = new CSingleDocTemplate(
      IDR_MAINFRAME,
      RUNTIME_CLASS(CMakeGifDoc),
      RUNTIME_CLASS(CMainFrame),       // main SDI frame window
      RUNTIME_CLASS(CMakeGifView));
  AddDocTemplate(pDocTemplate);

  CCommandLineInfo cmdInfo;
  ParseCommandLine(cmdInfo);

  if(!ProcessShellCommand(cmdInfo)) {
    return FALSE;
  }
  m_pMainWnd->ShowWindow(SW_SHOW);
  m_pMainWnd->UpdateWindow();

  return TRUE;
}


class CAboutDlg : public CDialog {
public:
    CAboutDlg();

    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);

protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

void CMakeGifApp::OnAppAbout() {
  CAboutDlg aboutDlg;
  aboutDlg.DoModal();
}

String CMakeGifApp::getRecentFile(int index) {
  CRecentFileList &list = *m_pRecentFileList;
  if(index >= list.GetSize()) {
    return EMPTYSTRING;
  }
  CString name = list[index];
  return name.GetBuffer(name.GetLength());
}

void CMakeGifApp::removeFromRecentFile(int index) {
  CRecentFileList &list = *m_pRecentFileList;
  if(index < list.GetSize()) {
    list.Remove(index);
  }
}
