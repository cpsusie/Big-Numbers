#include "stdafx.h"
#include <Process.h>
#include "Fern.h"
#include "FernDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog {
public:
  CAboutDlg();

  enum { IDD = IDD_ABOUTBOX };

protected:
  virtual void DoDataExchange(CDataExchange *pDX);

protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CFernDlg::CFernDlg(CWnd *pParent) : CDialog(CFernDlg::IDD, pParent) {
  m_timerIsRunning = false;
  m_x00 = 0.0;
  m_x01 = 0.0;
  m_x02 = 0.0;
  m_x03 = 0.0;
  m_x04 = 0.0;
  m_x05 = 0.0;
  m_x06 = 0.0;
  m_x10 = 0.0;
  m_x11 = 0.0;
  m_x12 = 0.0;
  m_x13 = 0.0;
  m_x14 = 0.0;
  m_x15 = 0.0;
  m_x16 = 0.0;
  m_x20 = 0.0;
  m_x21 = 0.0;
  m_x22 = 0.0;
  m_x23 = 0.0;
  m_x24 = 0.0;
  m_x25 = 0.0;
  m_x26 = 0.0;
  m_x30 = 0.0;
  m_x31 = 0.0;
  m_x32 = 0.0;
  m_x33 = 0.0;
  m_x34 = 0.0;
  m_x35 = 0.0;
  m_x36 = 0.0;
  m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);
}

void CFernDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDIT00, m_x00);
  DDX_Text(pDX, IDC_EDIT01, m_x01);
  DDX_Text(pDX, IDC_EDIT02, m_x02);
  DDX_Text(pDX, IDC_EDIT03, m_x03);
  DDX_Text(pDX, IDC_EDIT04, m_x04);
  DDX_Text(pDX, IDC_EDIT05, m_x05);
  DDX_Text(pDX, IDC_EDIT06, m_x06);
  DDX_Text(pDX, IDC_EDIT10, m_x10);
  DDX_Text(pDX, IDC_EDIT11, m_x11);
  DDX_Text(pDX, IDC_EDIT12, m_x12);
  DDX_Text(pDX, IDC_EDIT13, m_x13);
  DDX_Text(pDX, IDC_EDIT14, m_x14);
  DDX_Text(pDX, IDC_EDIT15, m_x15);
  DDX_Text(pDX, IDC_EDIT16, m_x16);
  DDX_Text(pDX, IDC_EDIT20, m_x20);
  DDX_Text(pDX, IDC_EDIT21, m_x21);
  DDX_Text(pDX, IDC_EDIT22, m_x22);
  DDX_Text(pDX, IDC_EDIT23, m_x23);
  DDX_Text(pDX, IDC_EDIT24, m_x24);
  DDX_Text(pDX, IDC_EDIT25, m_x25);
  DDX_Text(pDX, IDC_EDIT26, m_x26);
  DDX_Text(pDX, IDC_EDIT30, m_x30);
  DDX_Text(pDX, IDC_EDIT31, m_x31);
  DDX_Text(pDX, IDC_EDIT32, m_x32);
  DDX_Text(pDX, IDC_EDIT33, m_x33);
  DDX_Text(pDX, IDC_EDIT34, m_x34);
  DDX_Text(pDX, IDC_EDIT35, m_x35);
  DDX_Text(pDX, IDC_EDIT36, m_x36);
}

BEGIN_MESSAGE_MAP(CFernDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_TIMER()
    ON_WM_SIZE()
    ON_WM_CLOSE()
    ON_COMMAND(ID_FILE_EXIT , OnFileExit  )
    ON_COMMAND(ID_HELP_ABOUT, OnHelpAbout )
    ON_EN_UPDATE(IDC_EDIT00 , OnUpdateEdit)
    ON_EN_UPDATE(IDC_EDIT01 , OnUpdateEdit)
    ON_EN_UPDATE(IDC_EDIT02 , OnUpdateEdit)
    ON_EN_UPDATE(IDC_EDIT03 , OnUpdateEdit)
    ON_EN_UPDATE(IDC_EDIT04 , OnUpdateEdit)
    ON_EN_UPDATE(IDC_EDIT05 , OnUpdateEdit)
    ON_EN_UPDATE(IDC_EDIT06 , OnUpdateEdit)
    ON_EN_UPDATE(IDC_EDIT10 , OnUpdateEdit)
    ON_EN_UPDATE(IDC_EDIT11 , OnUpdateEdit)
    ON_EN_UPDATE(IDC_EDIT12 , OnUpdateEdit)
    ON_EN_UPDATE(IDC_EDIT13 , OnUpdateEdit)
    ON_EN_UPDATE(IDC_EDIT14 , OnUpdateEdit)
    ON_EN_UPDATE(IDC_EDIT15 , OnUpdateEdit)
    ON_EN_UPDATE(IDC_EDIT16 , OnUpdateEdit)
    ON_EN_UPDATE(IDC_EDIT20 , OnUpdateEdit)
    ON_EN_UPDATE(IDC_EDIT21 , OnUpdateEdit)
    ON_EN_UPDATE(IDC_EDIT22 , OnUpdateEdit)
    ON_EN_UPDATE(IDC_EDIT23 , OnUpdateEdit)
    ON_EN_UPDATE(IDC_EDIT24 , OnUpdateEdit)
    ON_EN_UPDATE(IDC_EDIT25 , OnUpdateEdit)
    ON_EN_UPDATE(IDC_EDIT26 , OnUpdateEdit)
    ON_EN_UPDATE(IDC_EDIT30 , OnUpdateEdit)
    ON_EN_UPDATE(IDC_EDIT31 , OnUpdateEdit)
    ON_EN_UPDATE(IDC_EDIT32 , OnUpdateEdit)
    ON_EN_UPDATE(IDC_EDIT33 , OnUpdateEdit)
    ON_EN_UPDATE(IDC_EDIT34 , OnUpdateEdit)
    ON_EN_UPDATE(IDC_EDIT35 , OnUpdateEdit)
    ON_EN_UPDATE(IDC_EDIT36 , OnUpdateEdit)
    ON_BN_CLICKED(IDC_BUTTONRESET, OnButtonReset)
END_MESSAGE_MAP()

#if defined(BREGNE)

static AfinType startAfin[AFIN_COUNT] = {
  { 0.00, 0.00, 0.00, 0.16, 0.00, 0.00, 0.01 },
  { 0.85, 0.06,-0.08, 0.85, 0.00, 1.60, 0.85 },
  { 0.20,-0.26, 0.23, 0.22, 0.00, 1.60, 0.07 },
  {-0.15, 0.28, 0.26, 0.24, 0.00, 0.44, 0.07 }
};

#else

static AfinType startAfin[AFIN_COUNT] = {
  { 0.05, 0.00, 0.00, 0.50, 0.00, 0.00, 0.33 },
  { 0.50, 0.00, 0.00, 0.50, 1.00, 0.00, 0.33 },
  { 0.50, 0.00, 0.00, 0.50, 0.50, 0.50, 0.34 }
};

#endif

void CFernDlg::initAfin() {
  for(int i = 0; i < AFIN_COUNT; i++) {
    for(int j = 0; j < 7; j++) {
      m_afin[i][j] = startAfin[i][j];
    }
  }
}

void CFernDlg::initProbability() {
  m_akkumulatedProbabilities[0] = m_afin[0][6];
  for(int t = 1; t < AFIN_COUNT; t++) {
    m_akkumulatedProbabilities[t] = m_akkumulatedProbabilities[t-1] + m_afin[t][6];
  }
}

Point2D CFernDlg::newPoint(AfinType a, const Point2D &point) {
  Point2D result;
  result.x = a[0] * point.x + a[1] * point.y + a[4];
  result.y = a[2] * point.x + a[3] * point.y + a[5];
  return result;
}

void CFernDlg::setNewPoint() {
  CClientDC dc(this);
  Viewport2D vp(dc,m_tr,true);
  for(int i = 0; i < 100; i++) {
    double r = (double)rand() / RAND_MAX;
    int t;
    for(t = 0; t < AFIN_COUNT && r > m_akkumulatedProbabilities[t]; t++);
    m_currentPoint = newPoint(m_afin[t], m_currentPoint);
    vp.SetPixel(m_currentPoint,RGB(0,128,0));
  }
}

void CFernDlg::afinToFields() {
  m_x00 = m_afin[0][0];
  m_x01 = m_afin[0][1];
  m_x02 = m_afin[0][2];
  m_x03 = m_afin[0][3];
  m_x04 = m_afin[0][4];
  m_x05 = m_afin[0][5];
  m_x06 = m_afin[0][6];

  m_x10 = m_afin[1][0];
  m_x11 = m_afin[1][1];
  m_x12 = m_afin[1][2];
  m_x13 = m_afin[1][3];
  m_x14 = m_afin[1][4];
  m_x15 = m_afin[1][5];
  m_x16 = m_afin[1][6];

  m_x20 = m_afin[2][0];
  m_x21 = m_afin[2][1];
  m_x22 = m_afin[2][2];
  m_x23 = m_afin[2][3];
  m_x24 = m_afin[2][4];
  m_x25 = m_afin[2][5];
  m_x26 = m_afin[2][6];

  m_x30 = m_afin[3][0];
  m_x31 = m_afin[3][1];
  m_x32 = m_afin[3][2];
  m_x33 = m_afin[3][3];
  m_x34 = m_afin[3][4];
  m_x35 = m_afin[3][5];
  m_x36 = m_afin[3][6];
}

void CFernDlg::fieldsToAfin() {
  m_afin[0][0] = m_x00 ;
  m_afin[0][1] = m_x01;
  m_afin[0][2] = m_x02;
  m_afin[0][3] = m_x03;
  m_afin[0][4] = m_x04;
  m_afin[0][5] = m_x05;
  m_afin[0][6] = m_x06;

  m_afin[1][0] = m_x10;
  m_afin[1][1] = m_x11;
  m_afin[1][2] = m_x12;
  m_afin[1][3] = m_x13;
  m_afin[1][4] = m_x14;
  m_afin[1][5] = m_x15;
  m_afin[1][6] = m_x16;

  m_afin[2][0] = m_x20;
  m_afin[2][1] = m_x21;
  m_afin[2][2] = m_x22;
  m_afin[2][3] = m_x23;
  m_afin[2][4] = m_x24;
  m_afin[2][5] = m_x25;
  m_afin[2][6] = m_x26;

  m_afin[3][0] = m_x30;
  m_afin[3][1] = m_x31;
  m_afin[3][2] = m_x32;
  m_afin[3][3] = m_x33;
  m_afin[3][4] = m_x34;
  m_afin[3][5] = m_x35;
  m_afin[3][6] = m_x36;

  initProbability();
}

BOOL CFernDlg::OnInitDialog() {
  __super::OnInitDialog();

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

  SetIcon(m_hIcon, TRUE);
  SetIcon(m_hIcon, FALSE);

  initAfin();
  initProbability();
  m_currentPoint.x = m_currentPoint.y = 0.0;
  afinToFields();

  UpdateData(false);
  m_tr.setToRectangle(Rectangle2DR(getClientRect(this)));
  m_tr.setFromRectangle(Rectangle2D(MINX, MAXY, MAXX-MINX, MINY-MAXY));

  startTimer();

  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFernDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

void CFernDlg::OnPaint() {
  if (IsIconic()) {
    CPaintDC dc(this);

    SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

    // Center icon in client rectangle
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

HCURSOR CFernDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

void CFernDlg::startTimer() {
  if(m_timerIsRunning) {
    return;
  }
  if(SetTimer(1,0,NULL) == 0) {
    showWarning(_T("Cannot install timer"));
  } else {
    m_timerIsRunning = true;
  }
}

void CFernDlg::stopTimer() {
  if(m_timerIsRunning) {
    KillTimer(1);
  }
  m_timerIsRunning = false;
}

void CFernDlg::OnOK() {
}
void CFernDlg::OnCancel() {
}
void CFernDlg::OnClose() {
  OnFileExit();
}

void CFernDlg::OnTimer(UINT_PTR nIDEvent) {
  setNewPoint();
  __super::OnTimer(nIDEvent);
}

void CFernDlg::OnSize(UINT nType, int cx, int cy) {
  CClientDC dc(this);

//  dc.SelectObject(::GetSysColor(COLOR_BTNFACE));
//  dc.Rectangle(0,0,rect.Width(),rect.Height());
//  dc.SetBkColor(RGB(0,200,0));

  m_tr.setToRectangle(Rectangle2DR(getClientRect(this)));
  __super::OnSize(nType, cx, cy);
  Invalidate();
}

void CFernDlg::OnFileExit() {
  EndDialog(IDOK);
}

void CFernDlg::OnHelpAbout() {
  CAboutDlg().DoModal();
}

void CFernDlg::OnUpdateEdit() {
  UpdateData();
  fieldsToAfin();
  Invalidate();
}

void CFernDlg::OnButtonReset() {
  initAfin();
  afinToFields();
  UpdateData(false);
  Invalidate();
}
