#include "stdafx.h"
#include <Process.h>
#include "MainFrm.h"
#include "CaptureDoc.h"
#include "CaptureView.h"
#include <comdef.h>
#include <atlconv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CCaptureApp, CWinApp)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
    ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

CCaptureApp::CCaptureApp() {
}

CCaptureApp theApp;

BOOL CCaptureApp::InitInstance() {

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);


	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit()) {
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);


  SetRegistryKey(_T("JGMData"));

  LoadStdProfileSettings(16);  // Load standard INI file options (including MRU)

  CSingleDocTemplate* pDocTemplate;
  pDocTemplate = new CSingleDocTemplate(
      IDR_MAINFRAME,
      RUNTIME_CLASS(CCaptureDoc),
      RUNTIME_CLASS(CMainFrame),
      RUNTIME_CLASS(CCaptureView));
  AddDocTemplate(pDocTemplate);

  CCommandLineInfo cmdInfo;
//  ParseCommandLine(cmdInfo);

  if(!ProcessShellCommand(cmdInfo)) {
    return FALSE;
  }

  m_pMainWnd->ShowWindow(SW_SHOW);
  m_pMainWnd->UpdateWindow();
  m_device.attach(m_pMainWnd->m_hWnd);
  return TRUE;
}

class CAboutDlg : public CDialog {
public:
  CAboutDlg();

  enum { IDD = IDD_ABOUTBOX };

public:
      virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
      virtual void DoDataExchange(CDataExchange* pDX);

protected:
    virtual BOOL OnInitDialog();
  DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
}

BOOL CAboutDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  GetDlgItem(IDC_EDITHOMEPAGE)->SetWindowText(_T("http://www.mikkelsenware.dk"));
  return TRUE;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

void CCaptureApp::OnAppAbout() {
  CAboutDlg aboutDlg;
  aboutDlg.DoModal();
}

static const char *browsers[] = {
  "C:\\Program Files\\Mozilla Firefox\\firefox"
 ,"C:\\Program Files\\Google\\Chrome\\Application\\chrome"
 ,"C:\\Program Files\\Internet Explorer\\iexplore"
};

BOOL CAboutDlg::PreTranslateMessage(MSG* pMsg) {
  USES_CONVERSION;
  switch(pMsg->message) {
  case WM_LBUTTONDBLCLK:
    { CEdit *homePageField = (CEdit*)GetDlgItem(IDC_EDITHOMEPAGE);
      if(getWindowRect(homePageField).PtInRect(pMsg->pt)) {
        TCHAR fieldText[256];
        homePageField->GetWindowText(fieldText,sizeof(fieldText));
        const String url = substr(fieldText,7,40);
        const char *urlA = T2A(url.cstr());
        for(int i = 0; i < ARRAYSIZE(browsers); i++) {
          const char *browser = browsers[i];
          String fileName = FileNameSplitter(browser).getFileName();
          const char *fileNameA = T2A(fileName.cstr());
          if(_spawnlp(_P_NOWAITO,browser,fileNameA,urlA,NULL) >= 0) {
            break;
          }
        }
        return true;
      }
    }
    break;
  }

  return CDialog::PreTranslateMessage(pMsg);
}
