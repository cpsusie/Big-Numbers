#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include <Process.h>
#include <FileNameSplitter.h>
#include "MainFrm.h"
#include "CaptureDoc.h"
#include "CaptureView.h"
#include <comdef.h>
#include <atlconv.h>

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CCaptureApp, CWinApp)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
    ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

CCaptureApp::CCaptureApp() {
    m_bHiColorIcons = TRUE;
    SetAppID(_T("Capture"));
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

    CWinAppEx::InitInstance();

    // Initialize OLE libraries
    if (!AfxOleInit()) {
        AfxMessageBox(IDP_OLE_INIT_FAILED);
        return FALSE;
    }

    AfxEnableControlContainer();

    EnableTaskbarInteraction(FALSE);

    SetRegistryKey(_T("JGMData"));

    LoadStdProfileSettings(16);  // Load standard INI file options (including MRU)

    InitContextMenuManager();

    InitKeyboardManager();

    InitTooltipManager();
    CMFCToolTipInfo ttParams;
    ttParams.m_bVislManagerTheme = TRUE;
    theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
        RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

    // Register the application's document templates.  Document templates
    //  serve as the connection between documents, frame windows and views
  CSingleDocTemplate* pDocTemplate;
  pDocTemplate = new CSingleDocTemplate(
      IDR_MAINFRAME,
      RUNTIME_CLASS(CCaptureDoc),
      RUNTIME_CLASS(CMainFrame),
      RUNTIME_CLASS(CCaptureView));
  if (!pDocTemplate)
      return FALSE;
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

int CCaptureApp::ExitInstance() {
  AfxOleTerm(FALSE);
  return CWinAppEx::ExitInstance();
}

class CAboutDlg : public CDialog {
public:
  enum { IDD = IDD_ABOUTBOX };
  CAboutDlg() : CDialog(IDD) {
  }

  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual BOOL OnInitDialog();
  DECLARE_MESSAGE_MAP()
};

BOOL CAboutDlg::OnInitDialog() {
  __super::OnInitDialog();
  GetDlgItem(IDC_EDITHOMEPAGE)->SetWindowText(_T("http://www.mikkelsenware.dk"));
  return TRUE;
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

void CCaptureApp::OnAppAbout() {
  CAboutDlg().DoModal();
}

// CMFCApplication2App customization load/save methods
void CCaptureApp::PreLoadState() {
  BOOL bNameValid;
  CString strName;
  bNameValid = strName.LoadString(IDS_EDIT_MENU);
  ASSERT(bNameValid);
  GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CCaptureApp::LoadCustomState() {
}

void CCaptureApp::SaveCustomState() {
}

static const char *browsers[] = {
  "C:\\Program Files\\Mozilla Firefox\\firefox"
 ,"C:\\Program Files\\Google\\Chrome\\Application\\chrome"
 ,"C:\\Program Files\\Internet Explorer\\iexplore"
};

BOOL CAboutDlg::PreTranslateMessage(MSG *pMsg) {
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
  return __super::PreTranslateMessage(pMsg);
}
