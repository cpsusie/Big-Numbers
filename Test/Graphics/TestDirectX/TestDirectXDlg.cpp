#include "stdafx.h"
#include "TestDirectXDlg.h"

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

CTestDirectXDlg::CTestDirectXDlg(CWnd *pParent /*=NULL*/) : CDialog(CTestDirectXDlg::IDD, pParent) {
  DEBUGTRACE;

    m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);
}

void CTestDirectXDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestDirectXDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_COMMAND(ID_FILE_EXIT, OnFileExit)
    ON_WM_CLOSE()
    ON_WM_SIZING()
END_MESSAGE_MAP()

BOOL CTestDirectXDlg::OnInitDialog() {
DEBUGTRACE;
  __super::OnInitDialog();

  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu *pSysMenu = GetSystemMenu(FALSE);
  if (pSysMenu != NULL) {
    CString strAboutMenu;
    strAboutMenu.LoadString(IDS_ABOUTBOX);
    if (!strAboutMenu.IsEmpty()) {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  theApp.m_device.attach(m_hWnd);

DEBUGTRACE;
  m_image = PixRect::load(theApp.m_device, ByteInputFile(_T("c:\\mytools2015\\spil\\chess\\res\\board.jpg")));
DEBUGTRACE;

  setClientRectSize(this, m_image->getSize());

  SetIcon(m_hIcon, TRUE);         // Set big icon
  SetIcon(m_hIcon, FALSE);        // Set small icon
  
  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestDirectXDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

#define ORIGIN CPoint(0,0)
void CTestDirectXDlg::OnPaint() {
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
    try {
      __super::OnPaint();
      CClientDC dc(this);
      CRect cr = getClientRect(this);
DEBUGTRACE;
static       PixRect             *screen = new PixRect(theApp.m_device);
DEBUGTRACE;
static       PixRect             *tmp = new PixRect(theApp.m_device, PIXRECT_PLAINSURFACE, getScreenSize());
//static const Array<DDSURFACEDESC2> displayModes = screen->getDisplayModes();
//             DDCAPS               driverCaps   = PixRect::getDriverCaps();
//             DDCAPS               emulCaps     = PixRect::getEmulatorCaps();

DEBUGTRACE;
      tmp->rop(cr, SRCCOPY, m_image, CRect(ORIGIN, m_image->getSize()));
DEBUGTRACE;
      PixRect::bitBlt(dc, ORIGIN, cr.Size(), SRCCOPY, tmp, ORIGIN);
DEBUGTRACE;
/*
      for(int i = 0; i < displayModes.size(); i++) {
        const DDSURFACEDESC2 &sd = displayModes[i];
        dc.TextOut(0,i*10, format(_T("flags:%08x caps:%08x"), sd.dwFlags, sd.ddsCaps.dwCaps).cstr());
      }
*/
    } catch(Exception e) {
      AfxMessageBox(e.what());
    }
  }
}

HCURSOR CTestDirectXDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

void CTestDirectXDlg::OnFileExit() {
  EndDialog(IDOK);
}

void CTestDirectXDlg::OnClose() {
  OnFileExit();
}

void CTestDirectXDlg::OnOK() { /* ignore */ }
void CTestDirectXDlg::OnCancel() { /* ignore */ }

void CTestDirectXDlg::OnSizing(UINT fwSide, LPRECT pRect) {
  __super::OnSizing(fwSide, pRect);
  Invalidate(false);
}
