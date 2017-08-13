#include "stdafx.h"
#include "testtooltipDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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

CTesttooltipDlg::CTesttooltipDlg(CWnd *pParent) : CDialog(CTesttooltipDlg::IDD, pParent) {
  m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);
}

void CTesttooltipDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTesttooltipDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xFFFF, OnToolTipNotify)
END_MESSAGE_MAP()

BOOL CTesttooltipDlg::OnInitDialog() {
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

  SetIcon(m_hIcon, TRUE);         // Set big icon
  SetIcon(m_hIcon, FALSE);        // Set small icon

  EnableToolTips();

  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTesttooltipDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

void CTesttooltipDlg::OnPaint() {
  if (IsIconic()) {
    CPaintDC dc(this); // device context for painting

    SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

    // Center icon in client rectangle
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    // Draw the icon
    dc.DrawIcon(x, y, m_hIcon);
  } else {
    __super::OnPaint();
  }
}

HCURSOR CTesttooltipDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

void CTesttooltipDlg::OnButton1() {
  showInformation(_T("this is Button 1"));
}

BOOL CTesttooltipDlg::OnToolTipNotify(UINT id, NMHDR *pNMHDR, LRESULT *pResult) {
  TOOLTIPTEXT *pTTT = (TOOLTIPTEXT*)pNMHDR; // Get the tooltip structure.
  UINT_PTR CtrlHandle = pNMHDR->idFrom; // Actually the idFrom holds Control's handle.

  // Check once again that the idFrom holds handle itself.
  if(pTTT->uFlags & TTF_IDISHWND) {
    UINT nID = ::GetDlgCtrlID(HWND(CtrlHandle));

#if (_MFC_VER < 0x0700)
    _AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
#else
     AFX_MODULE_THREAD_STATE *pThreadState = AfxGetModuleThreadState();
#endif
    CToolTipCtrl *pToolTip = pThreadState->m_pToolTip;
    if(pToolTip) {
      pToolTip->SetMaxTipWidth(SHRT_MAX); // Do this to make \r\n work!
    }

    // Now you have the ID. depends on control, set your tooltip message.
    switch(nID) {
    case IDC_BUTTON1:
      pTTT->lpszText = _T("First Button\r\nThis is a longer text\r\nwith 3 lines.");
      break;

    default:
      pTTT->lpszText = _T("Tooltips everywhere!!!");
      break;
    }
    return TRUE;
  }
  return FALSE;
}
