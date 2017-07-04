#include "stdafx.h"
#include "MainFrm.h"
#include "MyPaintDoc.h"
#include "MyPaintView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CMyPaintApp, CWinApp)
  ON_COMMAND(ID_APP_ABOUT       , OnAppAbout      )
  ON_COMMAND(ID_FILE_NEW        , OnFileNew       )
  ON_COMMAND(ID_FILE_OPEN       , OnFileOpen      )
  ON_COMMAND(ID_FILE_PRINT_SETUP, OnFilePrintSetup)
END_MESSAGE_MAP()

CMyPaintApp::CMyPaintApp() {
}

CMyPaintApp theApp;

BOOL CMyPaintApp::InitInstance() {
  INITCOMMONCONTROLSEX InitCtrls;
  InitCtrls.dwSize = sizeof(InitCtrls);
  // Set this to include all the common control classes you want to use
  // in your application.
  InitCtrls.dwICC = ICC_WIN95_CLASSES;
  InitCommonControlsEx(&InitCtrls);

  __super::InitInstance();

  EnableTaskbarInteraction(FALSE);

  // Change the registry key under which our settings are stored.
  SetRegistryKey(_T("JGMData"));

  LoadStdProfileSettings(16);  // Load standard INI file options (including MRU)

  CSingleDocTemplate *pDocTemplate;
  pDocTemplate = new CSingleDocTemplate(
      IDR_MAINFRAME,
      RUNTIME_CLASS(CMyPaintDoc),
      RUNTIME_CLASS(CMainFrame),       // main SDI frame window
      RUNTIME_CLASS(CMyPaintView));
  AddDocTemplate(pDocTemplate);

  CCommandLineInfo cmdInfo;
  ParseCommandLine(cmdInfo);

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

String CMyPaintApp::getRecentFile(int index) {
  CRecentFileList &list = *m_pRecentFileList;
  if(index >= list.GetSize()) {
    return EMPTYSTRING;;
  }
  return (LPCTSTR)list[index];
}

void CMyPaintApp::removeRecentFile(int index) {
  CRecentFileList &list = *m_pRecentFileList;
  if((UINT)index < (UINT)list.GetSize()) {
    return list.Remove(index);
  }
}

PixRect *CMyPaintApp::fetchPixRect(const CSize &size) {
  PixRect *pr = new PixRect(m_device, PIXRECT_PLAINSURFACE, size);
  pr->fillColor(WHITE);
  return pr;
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

void CMyPaintApp::OnAppAbout() {
  CAboutDlg().DoModal();
}
