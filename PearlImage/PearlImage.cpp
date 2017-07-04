#include "stdafx.h"
#include <afxadv.h>

#include "MainFrm.h"
#include "PearlImageDoc.h"
#include "PearlImageView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CPearlImageApp, CWinApp)
  ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
  ON_COMMAND(ID_FILE_NEW,  OnFileNew)
  ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
  ON_COMMAND(ID_FILE_PRINT_SETUP, OnFilePrintSetup)
END_MESSAGE_MAP()

CPearlImageApp::CPearlImageApp() {
}

CPearlImageApp theApp;

BOOL CPearlImageApp::InitInstance() {
  AfxEnableControlContainer();

  // Change the registry key under which our settings are stored.
  SetRegistryKey(_T("JGMData"));

  LoadStdProfileSettings(16);  // Load standard INI file options (including MRU)

  // Register document templates

  CSingleDocTemplate *pDocTemplate;
  pDocTemplate = new CSingleDocTemplate(
      IDR_MAINFRAME,
      RUNTIME_CLASS(CPearlImageDoc),
      RUNTIME_CLASS(CMainFrame),       // main SDI frame window
      RUNTIME_CLASS(CPearlImageView));
  AddDocTemplate(pDocTemplate);

  // Parse command line for standard shell commands, DDE, file open
  CCommandLineInfo cmdInfo;
  ParseCommandLine(cmdInfo);

  // Dispatch commands specified on the command line
  if(!ProcessShellCommand(cmdInfo)) {
    return FALSE;
  }

  TCHAR **argv = __targv;
  argv++;
  if(*argv) {
    getMainFrame()->loadFile(*argv);
  }

  m_pMainWnd->ShowWindow(SW_SHOW);
  m_pMainWnd->UpdateWindow();

  return TRUE;
}

String CPearlImageApp::getRecentFile(int index) {
  CRecentFileList &list = *m_pRecentFileList;
  if(index >= list.GetSize())
    return EMPTYSTRING;
  CString name = list[index];
  return name.GetBuffer(name.GetLength());
}

class CAboutDlg : public CDialog {
public:
  CAboutDlg();

  enum { IDD = IDD_ABOUTBOX };

protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

void CPearlImageApp::OnAppAbout() {
  CAboutDlg().DoModal();
}
