#include "stdafx.h"
#include "TestMediaPlayerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog {
public:
  CAboutDlg();

  enum { IDD = IDD_ABOUTBOX };

  virtual void DoDataExchange(CDataExchange* pDX);

protected:
  DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX) {
  __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CTestMediaPlayerDlg::CTestMediaPlayerDlg(CWnd* pParent) : CDialog(CTestMediaPlayerDlg::IDD, pParent) {
  m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);
}

void CTestMediaPlayerDlg::DoDataExchange(CDataExchange* pDX) {
  __super::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_MEDIAPLAYER, m_player);
}

BEGIN_MESSAGE_MAP(CTestMediaPlayerDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTONPLAY, OnButtonplay)
    ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CTestMediaPlayerDlg::OnInitDialog() {
    __super::OnInitDialog();

    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL) {
        CString strAboutMenu;
        strAboutMenu.LoadString(IDS_ABOUTBOX);
        if (!strAboutMenu.IsEmpty()) {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    m_layoutManager.OnInitDialog(this);
    m_layoutManager.addControl(IDC_BUTTONPLAY , RELATIVE_X_POS);
    m_layoutManager.addControl(IDOK           , RELATIVE_X_POS);
    return TRUE;
}

void CTestMediaPlayerDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

void CTestMediaPlayerDlg::OnPaint() {
  if (IsIconic()) {
    CPaintDC dc(this);

    SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    dc.DrawIcon(x, y, m_hIcon);
  } else {
    __super::OnPaint();
  }
}

HCURSOR CTestMediaPlayerDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

void CTestMediaPlayerDlg::OnButtonplay() {
  static const TCHAR *loadFileDialogExtensions = _T("Musik filer (*.wav,*.mp3,*.wma)\0*.wav;*.mp3;*.wma;\0"
                                                    "All files (*.*)\0*.*;\0"
                                                    "\0");
  CFileDialog dlg(TRUE);
  dlg.m_ofn.lpstrFilter = loadFileDialogExtensions;
  dlg.m_ofn.lpstrTitle  = _T("Vælg fil");
  dlg.m_ofn.Flags |= OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_ENABLESIZING;

//  if(dlg.DoModal() == IDOK) {
//    m_player.setSetUrl(dlg.m_ofn.lpstrFile);
//  }
}

void CTestMediaPlayerDlg::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);
}
