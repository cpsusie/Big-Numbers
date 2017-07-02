#include "stdafx.h"
#include <process.h>
#include "heatCPUDlg.h"
#include <ByteFile.h>

#ifdef _DEBUG
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

CHeatCPUDlg::CHeatCPUDlg(CWnd *pParent) : CDialog(CHeatCPUDlg::IDD, pParent) {
    m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);
    m_timerIsRunning = false;
}

void CHeatCPUDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CHeatCPUDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_CLOSE()
    ON_WM_LBUTTONDOWN()
    ON_WM_TIMER()
    ON_COMMAND(ID_FILE_EXIT, OnFileExit)
    ON_COMMAND(ID_OPTIONS_LAUNCHATSTARTUP, OnOptionsLaunchAtStartup)
    ON_COMMAND(ID_OPTIONS_SHOWCOUNTERS, OnOptionsShowCounters)
    ON_WM_SHOWWINDOW()
    ON_WM_HSCROLL()
END_MESSAGE_MAP()

BOOL CHeatCPUDlg::OnInitDialog() {
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

  SetIcon(m_hIcon, TRUE );
  SetIcon(m_hIcon, FALSE);

#define SLIDERMAX 100
  SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);
  SetThreadPriorityBoost(GetCurrentThread(), TRUE);

  getSliderCtrl()->SetRange(0, SLIDERMAX);
  setCPULoad(m_options.m_cpuLoad);
  checkMenuItem(this, ID_OPTIONS_LAUNCHATSTARTUP, m_options.m_autoLaunch  );
  checkMenuItem(this, ID_OPTIONS_SHOWCOUNTERS   , m_options.m_showCounters);
  showCounters(m_options.m_showCounters);

  addStatusIcon();

  startTimer();
  m_paintCount = 0;
  return TRUE;
}

void CHeatCPUDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX)   {
    CAboutDlg().DoModal();
  } else {
    if(nID == SC_MINIMIZE) {
      hideWindow();
    } else {
      __super::OnSysCommand(nID, lParam);
    }
  }
}

void CHeatCPUDlg::OnPaint() {
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

    // Draw the icon
    dc.DrawIcon(x, y, m_hIcon);
  } else {
    if(m_paintCount++ < 1) {
      hideWindow();
    } else {
      __super::OnPaint();
    }
  }
}

void CHeatCPUDlg::setCPULoad(int pct) { // pct == [0..100]
  pct = minMax(pct, 0, SLIDERMAX);

  const int threadCount = ARRAYSIZE(m_CPUHeaterThread);
  const double loadPerThread = 1.0 / threadCount;
  double wantedLoad = (double)pct/100.0;
  double totalLoad  = 0;
  for(int i = 0; i < threadCount; i++) {
    const double threadLoad = minMax(wantedLoad-totalLoad, 0.0, loadPerThread);
    m_CPUHeaterThread[i].setCPULoad(threadCount*threadLoad);
    totalLoad += threadLoad;
  }

/*
  const int threadCount = ARRAYSIZE(m_CPUHeaterThread);
  double wantedLoadPerThread = (double)pct/100.0;
  for(int i = 0; i < threadCount; i++) {
    m_CPUHeaterThread[i].setCPULoad(wantedLoadPerThread);
  }
*/
  if(pct != m_options.m_cpuLoad) {
    m_options.m_cpuLoad = pct;
    m_options.save();
  }
  setWindowText(GetDlgItem(IDC_LOADTEXT), format(_T("%d%%"), pct));
  getSliderCtrl()->SetPos(pct);
  try {
    notifyIconSetToolTip(m_hWnd, 1, getToolTipText());
  } catch(Exception) {
    // ignore
  }
}

void CHeatCPUDlg::refreshAfterShutdown() {
  setCPULoad(m_options.m_cpuLoad);
}

String CHeatCPUDlg::getToolTipText() const {
  return format(_T("CPU heater %d%%"), m_options.m_cpuLoad);
}

void CHeatCPUDlg::addStatusIcon() {
  notifyIconAdd(m_hWnd, 1, m_hIcon, getToolTipText());
}

void CHeatCPUDlg::deleteStatusIcon() {
  notifyIconDelete(m_hWnd, 1);
}

HCURSOR CHeatCPUDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

void CHeatCPUDlg::OnFileExit() {
  OnClose();
}

void CHeatCPUDlg::OnClose() {
  deleteStatusIcon();
  exit(0);
}

void CHeatCPUDlg::OnOK() {
}

void CHeatCPUDlg::OnCancel() {
  hideWindow();
}

void CHeatCPUDlg::hideWindow() {
  ShowWindow(SW_HIDE);
}

void CHeatCPUDlg::OnLButtonDown(UINT nFlags, CPoint point) {
  const MSG *msg = GetCurrentMessage();
  if(msg->lParam & 0x1) {
    ShowWindow(SW_SHOW);
    BringWindowToTop();
  }
}

void CHeatCPUDlg::OnTimer(UINT_PTR nIDEvent) {
  Timestamp now;
  if(diff(m_lastTimerEvent, now, TSECOND) > 10) {
    refreshAfterShutdown();
  }
  __super::OnTimer(nIDEvent);
  m_lastTimerEvent = Timestamp();
}

void CHeatCPUDlg::startTimer() {
  if(m_timerIsRunning) {
    return;
  }
  m_lastTimerEvent = Timestamp();
  m_timerIsRunning = true;
}

void CHeatCPUDlg::stopTimer() {
  if(m_timerIsRunning) {
    KillTimer(1);
  }
  m_timerIsRunning = false;
}

void copyFile(const String &srcName, const String &dstName) {
  ByteOutputFile dst(dstName);
  ByteInputFile  src(srcName);
  BYTE buffer[4096];
  intptr_t n;
  while((n = src.getBytes(buffer, sizeof(buffer))) > 0) {
    dst.putBytes(buffer, n);
  }
}

void CHeatCPUDlg::OnOptionsLaunchAtStartup() {
  const bool autoLaunch = toggleMenuItem(this, ID_OPTIONS_LAUNCHATSTARTUP);

  if(autoLaunch) {
    const String           userName      = getUserName();
    const String           startupFolder = format(_T("C:\\Users\\%s\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Startup"), userName.cstr());
    const FileNameSplitter info(getModuleFileName());
    const String           thisProgram   = info.getFileName() + info.getExtension();
    const String           dstFileName   = FileNameSplitter::getChildName(startupFolder, thisProgram);
    try {
      copyFile(getModuleFileName(), dstFileName);
      m_options.m_autoLaunch = true;
      m_options.save();
    } catch(Exception e) {
      MessageBox(e.what(), _T("Error"), MB_ICONEXCLAMATION);
    }
  }
}

void CHeatCPUDlg::showCounters(bool show) {
  if(show) {
    setClientRectSize(this, CSize(600, 200));
  } else {
    setClientRectSize(this, CSize(370, 90));
  }

  showCounterWindows(show);

  m_options.m_showCounters = show;
  m_options.save();
}

void CHeatCPUDlg::showCounterWindows(bool show) {
  int infoWnd[] =  { IDC_INFO1, IDC_INFO2, IDC_INFO3, IDC_INFO4 };
  for(int i = 0; i < ARRAYSIZE(m_CPUHeaterThread); i++) {
    CWnd *w = GetDlgItem(infoWnd[i]);
    w->ShowWindow(show ? SW_SHOW : SW_HIDE);
    m_CPUHeaterThread[i].setInfoWindow(show ? w : NULL);
  }
}

void CHeatCPUDlg::OnOptionsShowCounters() {
  showCounters(toggleMenuItem(this, ID_OPTIONS_SHOWCOUNTERS));
}

void CHeatCPUDlg::OnShowWindow(BOOL bShow, UINT nStatus) {
  __super::OnShowWindow(bShow, nStatus);
  showCounterWindows(bShow && m_options.m_showCounters);
}

BOOL CHeatCPUDlg::PreTranslateMessage(MSG *pMsg) {
  switch(pMsg->message) {
  case WM_MOUSEWHEEL:
    { short zDelta = (short)(pMsg->wParam >> 16);
      setCPULoad(m_options.m_cpuLoad + ((zDelta < 0)?-1:1));
      return true;
    }
  }
  return __super::PreTranslateMessage(pMsg);
}

void CHeatCPUDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
  setCPULoad(getSliderCtrl()->GetPos());
  __super::OnHScroll(nSBCode, nPos, pScrollBar);
}
