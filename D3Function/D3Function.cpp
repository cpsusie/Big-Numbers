#include "stdafx.h"
#include <afxadv.h>
#include "MainFrm.h"
#include "D3FunctionDoc.h"
#include "D3SceneView.h"

#ifdef _DEBUG
#include <DebugLog.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CD3FunctionApp, CWinApp)
  ON_COMMAND(ID_APP_ABOUT       , OnAppAbout      )
  ON_COMMAND(ID_FILE_OPEN       , OnFileOpen      )
  ON_COMMAND(ID_FILE_PRINT_SETUP, OnFilePrintSetup)
END_MESSAGE_MAP()

CD3FunctionApp::CD3FunctionApp() {
  SetAppID(_T("D3Function version 3.00"));
}

CD3FunctionApp theApp;

BOOL CD3FunctionApp::InitInstance() {
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

  LoadStdProfileSettings(_AFX_MRU_MAX_COUNT);  // Load standard INI file options (including MRU)

  // Register the application's document templates. Document templates
  // serve as the connection between documents, frame windows and views.

  CSingleDocTemplate *pDocTemplate = new CSingleDocTemplate(IDR_MAINFRAME
                                                           ,RUNTIME_CLASS(CD3FunctionDoc)
                                                           ,RUNTIME_CLASS(CMainFrame)       // main SDI frame window
                                                           ,RUNTIME_CLASS(C3DSceneView));
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

  HICON icon = LoadIcon(IDR_MAINFRAME);
  m_pMainWnd->SetIcon(icon,false);
  return TRUE;
}

int CD3FunctionApp::ExitInstance() {
  return __super::ExitInstance();
}

// -----------------------------------------------------------------------------

class CAboutDlg : public CDialog {
public:
  enum { IDD = IDD_ABOUTBOX };

  CAboutDlg() : CDialog(IDD) {
  }

protected:
  virtual BOOL OnInitDialog();
  DECLARE_MESSAGE_MAP()
};

BOOL CAboutDlg::OnInitDialog() {
  __super::OnInitDialog();
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// -----------------------------------------------------------------------------

void CD3FunctionApp::OnAppAbout() {
  CAboutDlg().DoModal();
}

void CD3FunctionApp::addToRecentFileList(LPCTSTR lpszPathName) {
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

String CD3FunctionApp::getRecentFile(int index) {
  CRecentFileList &list = *m_pRecentFileList;
  if(index >= list.GetSize()) {
    return EMPTYSTRING;
  }
  return (LPCTSTR)list[index];
}

void CD3FunctionApp::removeFromRecentFiles(int index) {
  CRecentFileList &list = *m_pRecentFileList;
  if(index >= list.GetSize()) {
    return;
  }
  list.Remove(index);
}

int D3ViewArray::findIndex(HWND hwnd) const {
  for (size_t i = 0; i < size(); i++) {
    C3DSceneView *view = (*this)[i];
    if (view->m_hWnd == hwnd) {
      return (int)i;
    }
  }
  return -1;
}

C3DSceneView *D3ViewArray::findViewByHwnd(HWND hwnd) const {
  const int index = findIndex(hwnd);
  return (index < 0) ? NULL : (*this)[index];
}

void D3ViewArray::remove(C3DSceneView *view) {
  const int index = findIndex(view->m_hWnd);
  if (index >= 0) {
    __super::remove(index);
  }
}
