#include "stdafx.h"
#include "PrShow.h"
#include <fcntl.h>
#include "MainFrm.h"
#include "ImageDoc.h"
#include "ImageView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CPrShowApp, CWinApp)
  ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
  ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
  ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
  ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

CPrShowApp::CPrShowApp() {
}

CPrShowApp theApp;

BOOL CPrShowApp::InitInstance() {
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
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

  LoadStdProfileSettings(_AFX_MRU_MAX_COUNT);

  CSingleDocTemplate *pDocTemplate;
  pDocTemplate = new CSingleDocTemplate(
      IDR_MAINFRAME,
      RUNTIME_CLASS(CImageDoc),
      RUNTIME_CLASS(CMainFrame),       // main SDI frame window
      RUNTIME_CLASS(CImageView));
  AddDocTemplate(pDocTemplate);

  CCommandLineInfo cmdInfo;
  ParseCommandLine(cmdInfo);

  if(!ProcessShellCommand(cmdInfo)) {
    return FALSE;
  }

  m_device.attach(getMainFrame()->m_hWnd);
  TCHAR **argv = __targv;
  argv++;
  String fileName;
  PixRect *pr;
  try {
    if(*argv != NULL) {
      fileName = *argv;
      pr = PixRect::load(m_device, ByteInputFile(fileName));
    } else {
      if(isatty(stdin)) {
        AfxMessageBox(_T("prShow:Cannot read image from keyboard"),MB_ICONSTOP);
        exit(-1);
      }

      fileName = _T("stdin");
      pr = PixRect::load(m_device, ByteInputFile(stdin));
    }
  } catch(Exception e) {
    AfxMessageBox(format(_T("prShow:%s:%s"), fileName.cstr(), e.what()).cstr(), MB_ICONSTOP);
    exit(-1);
  }

  getMainFrame()->getDocument()->setPixRect(pr);
  getMainFrame()->getDocument()->setName(fileName);
  getMainFrame()->setTitle();

  m_pMainWnd->ShowWindow(SW_SHOW);
  m_pMainWnd->UpdateWindow();

  centerWindow(m_pMainWnd);
  return TRUE;
}

class CAboutDlg : public CDialog {
public:
  CAboutDlg();

  enum { IDD = IDD_ABOUTBOX };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);
  DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX) {
  __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

void CPrShowApp::OnAppAbout() {
  CAboutDlg aboutDlg;
  aboutDlg.DoModal();
}
