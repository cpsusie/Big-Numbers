#include "stdafx.h"
#include "GetColorDlg.h"

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
  CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CGetcolorDlg::CGetcolorDlg(CWnd *pParent) : CDialog(CGetcolorDlg::IDD, pParent) {
  m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);
}

void CGetcolorDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGetcolorDlg, CDialog)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_WM_CANCELMODE()
END_MESSAGE_MAP()

BOOL CGetcolorDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu *pSysMenu = GetSystemMenu(FALSE);
  if(pSysMenu != NULL) {
    CString strAboutMenu;
    strAboutMenu.LoadString(IDS_ABOUTBOX);
    if(!strAboutMenu.IsEmpty()) {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  SetIcon(m_hIcon, TRUE);         // Set big icon
  SetIcon(m_hIcon, FALSE);        // Set small icon
/*
  for(int i = 0; ; i++) {
    DISPLAY_DEVICE displaydevice;
    displaydevice.cb = sizeof(displaydevice);
    if(EnumDisplayDevices(NULL,i,&displaydevice,0) == 0) break;
  }
*/
  hdcScreen     = CreateDC(_T("DISPLAY"), NULL, NULL, NULL); 
  scrw          = GetDeviceCaps(hdcScreen, HORZRES);
  scrh          = GetDeviceCaps(hdcScreen, VERTRES); 
  hdcCompatible = CreateCompatibleDC(hdcScreen); 

  hbmScreen = CreateCompatibleBitmap(hdcScreen, scrw, scrh);

  if(!SelectObject(hdcCompatible, hbmScreen)) {
    DeleteDC(hdcScreen);
    MessageBox(_T("SelecetObject failed"));
    exit(0);
  }

  if(!BitBlt(hdcCompatible, 0,0, scrw, scrh, hdcScreen, 0,0, SRCCOPY)) {
    DeleteDC(hdcScreen);
    MessageBox(_T("BitBlt failed:%s"),getLastErrorText().cstr());
    exit(0);
  }
  
  WINDOWPLACEMENT wp;
  GetWindowPlacement(&wp);
  wp.rcNormalPosition.left   = 0;
  wp.rcNormalPosition.top    = 0;
  wp.rcNormalPosition.right  = scrw;
  wp.rcNormalPosition.bottom = scrh;
  SetWindowPlacement(&wp);

  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGetcolorDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  } else {
    CDialog::OnSysCommand(nID, lParam);
  }
}

void CGetcolorDlg::OnPaint() {
  if(IsIconic()) {
    CPaintDC dc(this);

    SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

    const int   cxIcon = GetSystemMetrics(SM_CXICON);
    const int   cyIcon = GetSystemMetrics(SM_CYICON);
    const CRect rect   = getClientRect(this);
    const int   x      = (rect.Width() - cxIcon + 1) / 2;
    const int   y      = (rect.Height() - cyIcon + 1) / 2;

    dc.DrawIcon(x, y, m_hIcon);
  } else {
    CDialog::OnPaint();
    CClientDC dc(this);
    if(!BitBlt(dc.m_hDC,0,0, scrw, scrh, hdcCompatible, 0,0, SRCCOPY)) {
      DeleteDC(hdcScreen);
      MessageBox(_T("BitBlt failed:%s"), getLastErrorText().cstr());
      exit(0);
    }
  }
}

HCURSOR CGetcolorDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

void CGetcolorDlg::displayColor(const POINT &pt) {
  CStatic  *st = (CStatic*)GetDlgItem(IDC_STATIC);
  CClientDC stdc(st);
  CClientDC dc(this);
  COLORREF  c = dc.GetPixel(pt.x,pt.y);
  const int r = GetRValue(c);
  const int g = GetGValue(c);
  const int b = GetBValue(c);

  setWindowText(st, format(_T("      (r,g,b):(%d,%d,%d)"),r,g,b));
  CBrush brush;
  brush.CreateSolidBrush(c);
  CBrush *oldBrush = stdc.SelectObject(&brush);
  stdc.Rectangle(0,0,15,15);
  stdc.SelectObject(oldBrush);
}

BOOL CGetcolorDlg::PreTranslateMessage(MSG *pMsg) {
  switch(pMsg->message) {
  case WM_LBUTTONDOWN:
    displayColor(pMsg->pt);
    break;
  case WM_MOUSEMOVE:
    if(pMsg->wParam & MK_LBUTTON) {
      displayColor(pMsg->pt);
    }
    break;
  }
  return CDialog::PreTranslateMessage(pMsg);
}

void CGetcolorDlg::OnCancelMode() {
  CDialog::OnCancelMode();
}
