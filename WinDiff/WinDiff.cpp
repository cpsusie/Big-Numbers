#include "stdafx.h"
#include "WinDiff.h"

#include <afxadv.h>
#include "MainFrm.h"
#include "Options.h"
#include "WinDiffDoc.h"
#include "WinDiffView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CWinDiffApp, CWinApp)
  ON_COMMAND(ID_APP_ABOUT       , OnAppAbout      )
  ON_COMMAND(ID_FILE_OPEN       , OnFileOpen      )
  ON_COMMAND(ID_FILE_PRINT_SETUP, OnFilePrintSetup)
END_MESSAGE_MAP()

CWinDiffApp::CWinDiffApp() {
  SetAppID(_T("WinDiff version 3.00"));
}

CWinDiffApp theApp;

BOOL CWinDiffApp::InitInstance() {
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
  if(!AfxOleInit()) {
    AfxMessageBox(IDP_OLE_INIT_FAILED);
    return FALSE;
  }

  AfxEnableControlContainer();
  EnableTaskbarInteraction(FALSE);

  SetRegistryKey(_T("JGMData"));

  m_options = Options::getDefaultOptions();
  LoadStdProfileSettings(_AFX_MRU_MAX_COUNT);  // Load standard INI file options (including MRU)

  // Register the application's document templates. Document templates
  // serve as the connection between documents, frame windows and views.

  CSingleDocTemplate *pDocTemplate = new CSingleDocTemplate(IDR_MAINFRAME
                                                           ,RUNTIME_CLASS(CWinDiffDoc)
                                                           ,RUNTIME_CLASS(CMainFrame)       // main SDI frame window
                                                           ,RUNTIME_CLASS(CWinDiffView));
  AddDocTemplate(pDocTemplate);

  // Parse command line for standard shell commands, DDE, file open
  CCommandLineInfo cmdInfo;

  ParseCommandLine(cmdInfo);

  // Dispatch commands specified on the command line
  if(!ProcessShellCommand(cmdInfo)) {
    return FALSE;
  }

  ((CMainFrame*)m_pMainWnd)->activateOptions();
  // The one and only window has been initialized, so show and update it.
  m_pMainWnd->ShowWindow(SW_SHOW);
  m_pMainWnd->UpdateWindow();

  const TCHAR *f1,*f2;
  TCHAR **argv = __targv;
  const TCHAR *defaultFileName = _T("Untitled");
  argv++;
  if(!*argv) {
    f1 = defaultFileName;
  } else {
    f1 = *(argv++);
  }
  if(!*argv) {
    f2 = defaultFileName;
  } else {
    f2 = *argv;
  }

  setWindowText(theApp.GetMainWnd(), format(_T("Differences between %s and %s"),f1,f2));
  HICON icon = LoadIcon(IDR_MAINFRAME);
  m_pMainWnd->SetIcon(icon,false);
  return TRUE;
}

int CWinDiffApp::ExitInstance() {
  m_options.m_ignoreRegex   = false;
  m_options.m_ignoreColumns = false;
  m_options.setAsDefault();
  return __super::ExitInstance();
}

class CAboutDlg : public CDialog {
public:
  enum { IDD = IDD_ABOUTBOX };

  CAboutDlg() : CDialog(IDD) {
  }

protected:
  virtual BOOL OnInitDialog();
  DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog() {
  __super::OnInitDialog();

  GetDlgItem(IDC_STATICCOPYRIGHT)->SetWindowText(format(_T("Copyright %c 2007"), 169).cstr());
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CWinDiffApp::OnAppAbout() {
  CAboutDlg().DoModal();
}

void CWinDiffApp::addToRecentFileList(LPCTSTR lpszPathName) {
  try {
    // Call the base class
    __super::AddToRecentFileList(lpszPathName);
  } catch (CException *e) {
#ifdef _DEBUG
    TCHAR strCause[1000];
    e->GetErrorMessage(strCause, ARRAYSIZE(strCause));
    debugLog(_T("Exception in %s: %s.  -- Ignoring\n"), __TFUNCTION__, strCause);
#endif
    e->Delete();
  }
}

String CWinDiffApp::getRecentFile(int index) {
  CRecentFileList &list = *m_pRecentFileList;
  if(index >= list.GetSize()) {
    return EMPTYSTRING;
  }
  return (LPCTSTR)list[index];
}

void CWinDiffApp::removeFromRecentFiles(int index) {
  CRecentFileList &list = *m_pRecentFileList;
  if(index >= list.GetSize()) {
    return;
  }
  list.Remove(index);
}
