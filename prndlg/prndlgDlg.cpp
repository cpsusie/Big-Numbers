#include "stdafx.h"
#include "prndlg.h"
#include "prndlgDlg.h"
#include <winspool.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog {
public:
  CAboutDlg();

  enum { IDD = IDD_ABOUTBOX };
protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CPrndlgDlg::CPrndlgDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CPrndlgDlg::IDD, pParent)
{
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPrndlgDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPrndlgDlg, CDialog)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
  ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
END_MESSAGE_MAP()

BOOL CPrndlgDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu* pSysMenu = GetSystemMenu(FALSE);
  if (pSysMenu != NULL)
  {
    CString strAboutMenu;
    strAboutMenu.LoadString(IDS_ABOUTBOX);
    if (!strAboutMenu.IsEmpty())
    {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  SetIcon(m_hIcon, TRUE);			// Set big icon
  SetIcon(m_hIcon, FALSE);		// Set small icon
  
  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPrndlgDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
  }
  else {
    CDialog::OnSysCommand(nID, lParam);
  }
}

void CPrndlgDlg::OnPaint() {
  if (IsIconic()) {
    CPaintDC dc(this); // device context for painting

    SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    dc.DrawIcon(x, y, m_hIcon);
  } else {
    CDialog::OnPaint();
  }
}

HCURSOR CPrndlgDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

void CPrndlgDlg::OnButton1() {
  CPrintDialog dlg(true);
  if(dlg.DoModal() != IDOK) return;
  CString device = dlg.GetDeviceName();
  CString driver = dlg.GetDriverName();
  CString port = dlg.GetPortName();
  LPDEVMODE mode = dlg.GetDevMode();
  int source = mode->dmDefaultSource;
  String tmp = format(_T("device:%s driver:%s port:%s source:%d")
                      ,(LPCTSTR)device
                      ,(LPCTSTR)driver
                      ,(LPCTSTR)port
                      ,source
                      );
  MessageBox(tmp.cstr());
}

void CPrndlgDlg::OnButton3() {
  TCHAR m_szName[256],pszPrinterName[256];
  TCHAR szWindows[256];

// read PrinterPorts section from win.ini
// returned String should be of the form "driver,port,timeout,timeout", i.e. "winspool,LPT1:,15,45".

  GetProfileString(TEXT("Windows"),TEXT("Device"),_T(",,,"),m_szName, ARRAYSIZE(m_szName));
  MessageBox(m_szName);
  return;

  WriteProfileString(TEXT("Windows"),TEXT("Device"),pszPrinterName );
          // Notify all open applications of the change. Note, only applications that handle the message will recognize the change.
#if WM_WININICHANGE != WM_SETTINGCHANGE
          // Old message type; for windows 95
  SendNotifyMessage(HWND_BROADCAST,WM_WININICHANGE,0,(LPARAM)szWindows);
#endif
          // New message type
  ::SendNotifyMessage(HWND_BROADCAST,WM_SETTINGCHANGE,0,(LPARAM)szWindows);	
}
