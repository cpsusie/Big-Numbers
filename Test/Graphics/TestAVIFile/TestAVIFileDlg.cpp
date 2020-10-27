#include "stdafx.h"
#include "TestAVIFile.h"
#include "TestAVIFileDlg.h"
#include "FrameCountDlg.h"

#if defined(_DEBUG)
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

CTestAVIFileDlg::CTestAVIFileDlg(CWnd *pParent /*=nullptr*/) : CDialog(CTestAVIFileDlg::IDD, pParent) {
    m_hIcon         = theApp.LoadIcon(IDR_MAINFRAME);
    m_movieThread   = nullptr;
    m_frame         = nullptr;
    m_frameCount    = 100;
    m_timersRunning = 0;
}

void CTestAVIFileDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestAVIFileDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_COMMAND(ID_FILE_EXIT, OnFileExit)
    ON_WM_CLOSE()
    ON_COMMAND(ID_EDIT_MAKEAVIFILE, OnEditMakeAVIFile)
    ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CTestAVIFileDlg::OnInitDialog() {
  __super::OnInitDialog();

  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu *pSysMenu = GetSystemMenu(FALSE);
  if(pSysMenu != nullptr) {
    CString strAboutMenu;
    strAboutMenu.LoadString(IDS_ABOUTBOX);
    if(!strAboutMenu.IsEmpty())  {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  SetIcon(m_hIcon, TRUE);
  SetIcon(m_hIcon, FALSE);

  theApp.m_device.attach(m_hWnd);
  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestAVIFileDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

void CTestAVIFileDlg::OnPaint() {
  if(IsIconic()) {
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

HCURSOR CTestAVIFileDlg::OnQueryDragIcon() {
  return (HCURSOR) m_hIcon;
}

void CTestAVIFileDlg::OnOK() {
}

void CTestAVIFileDlg::OnCancel() {
}

void CTestAVIFileDlg::OnFileExit() {
  EndDialog(IDOK);
}

void CTestAVIFileDlg::OnClose() {
  OnFileExit();
}

#define isTimerRunning(id) (m_timersRunning &   (1 << (id)))
#define setTimerBit(id)    (m_timersRunning |=  (1 << (id)))
#define clrTimerBit(id)    (m_timersRunning &= ~(1 << (id)))

void CTestAVIFileDlg::startTimer(unsigned int id, int msec) {
  if(!isTimerRunning(id) && SetTimer(id,msec,nullptr)) {
    setTimerBit(id);
  }
}

void CTestAVIFileDlg::stopTimer(unsigned int id) {
  if(isTimerRunning(id)) {
    KillTimer(id);
    clrTimerBit(id);
  }
}

void CTestAVIFileDlg::OnTimer(UINT_PTR nIDEvent) {
  __super::OnTimer(nIDEvent);
  if(!m_movieThread->stillActive()) {
    stopMovieThread();
  }
  showState();
}

void CTestAVIFileDlg::showState() {
  if(m_movieThread) {
    setWindowText(this, format(_T("Frame %d/%d"), m_frameIndex, m_frameCount));
  } else {
    setWindowText(this, _T("Idle"));
  }
}


void CTestAVIFileDlg::drawFrame() {
  if(m_queue.size() == 20) {
    const LineElement l = m_queue.get();
    m_frame->line(l.m_p1,l.m_p2,WHITE);
  }
  m_frame->line(m_p1, m_p2, m_color);
  m_queue.put(LineElement(m_p1,m_p2));
  flushFrame();
  nextPoint(m_p1, m_step1);
  nextPoint(m_p2, m_step2);
  nextColor();
}

void CTestAVIFileDlg::flushFrame() {
  CClientDC dc(this);
  PixRect::bitBlt(dc,CPoint(0,0),m_frameSize, SRCCOPY, m_frame, CPoint(0,0));
}

void CTestAVIFileDlg::nextPoint(CPoint &p, CSize &step) {
  CPoint np = p + step;
  if(np.x < 0) {
    step.cx *= -1;
    np.x = p.x + step.cx;
  } else if(np.x >= m_frameSize.cx) {
    step.cx *= -1;
    np.x = p.x + step.cx;
  }
  if(np.y < 0) {
    step.cy *= -1;
    np.y = p.y + step.cy;
  } else if(np.y >= m_frameSize.cy) {
    step.cy *= -1;
    np.y = p.y + step.cy;
  }
  p = np;
}

void CTestAVIFileDlg::nextColor() {
  int r = ARGB_GETRED(  m_color);
  int g = ARGB_GETGREEN(m_color);
  int b = ARGB_GETBLUE( m_color);
  nextColorComponent(r, m_colorStep[0]);
  nextColorComponent(g, m_colorStep[1]);
  nextColorComponent(b, m_colorStep[2]);
  m_color = D3DCOLOR_XRGB(r,g,b);
}

void CTestAVIFileDlg::nextColorComponent(int &cc, int &step) {
  int nc = cc + step;
  if(nc >= 256) {
    step = -(int)randInt(7)+2;
    nc = cc + step;
  } else if(nc < 0) {
    step = randInt(7)+2;
    nc = cc + step;
  }
  cc = nc;
}

CSize CTestAVIFileDlg::getFrameSize() {
  return m_frameSize;
}

PixRect *CTestAVIFileDlg::nextFrame() {
  if(m_frameIndex < m_frameCount) {
    drawFrame();
    m_frameIndex++;
    return m_frame;
  } else {
    return nullptr;
  }
}

void CTestAVIFileDlg::OnEditMakeAVIFile() {
  CFrameCountDlg dlg(m_frameCount);
  if(dlg.DoModal() == IDOK) {
    startMovieThread(dlg.m_frameCount);
  }
}

void CTestAVIFileDlg::startMovieThread(UINT frameCount) {
  stopMovieThread();
  m_frameCount  = frameCount;
  m_frameSize   = getClientRect(this).Size();
  m_frameIndex  = 0;
  m_frame       = new PixRect(theApp.m_device, PIXRECT_PLAINSURFACE, m_frameSize); TRACE_NEW(m_frame);
  m_movieThread = new MovieThread(this); TRACE_NEW(m_movieThread);

  m_p1.x = randInt(m_frameSize.cx);
  m_p1.y = randInt(m_frameSize.cy);
  do {
    m_p2.x = randInt(m_frameSize.cx);
    m_p2.y = randInt(m_frameSize.cy);
  } while(distance(Point2D(m_p1), Point2D(m_p2)) < 30);

  m_step1.cx = randInt(20)-10;
  m_step1.cy = randInt(20)-10;
  do {
    m_step2.cx = randInt(20)-10;
    m_step2.cy = randInt(20)-10;
  } while(m_step2 == m_step1);

  m_color = D3DCOLOR_XRGB(randInt(256),randInt(256), randInt(256));
  for(int i = 0; i < 3; i++) {
    do {
      m_colorStep[i] = randInt(16)-8;
    } while(m_colorStep[i] == 0);
  }
  startTimer(1,500);
  m_movieThread->start();
}

void CTestAVIFileDlg::stopMovieThread() {
  if(m_movieThread) {
    m_frameIndex = m_frameCount;
    for(int i = 0; (i < 10) && m_movieThread->stillActive(); i++) {
      Sleep(100);
    }
    if(m_movieThread->stillActive()) {
      showWarning(_T("Cannot stop movieThread"));
    }
    SAFEDELETE(m_movieThread);
  }
  SAFEDELETE(m_frame);
  stopTimer(1);
  showState();
}
