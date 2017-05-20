#include "stdafx.h"
#include "TestBin.h"
#include "TestBinDlg.h"
#include "TimerDlg.h"

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

CTestbinDlg::CTestbinDlg(CWnd *pParent) : CDialog(CTestbinDlg::IDD, pParent) {
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestbinDlg::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestbinDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_STARTBUTTON, OnStartButton)
    ON_BN_CLICKED(IDC_STOPBUTTON, OnStopbutton)
    ON_WM_TIMER()
    ON_COMMAND(ID_FILE_EXIT, OnFileExit)
    ON_COMMAND(ID_VIEW_TIMER, OnViewTimer)
    ON_BN_CLICKED(IDC_STEPBUTTON, OnStepbutton)
    ON_BN_CLICKED(IDC_RESETBUTTON, OnResetbutton)
    ON_WM_CLOSE()
END_MESSAGE_MAP()


BOOL CTestbinDlg::OnInitDialog() {
    CDialog::OnInitDialog();

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

    SetIcon(m_hIcon, TRUE);         // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon

    m_counterThread  = new CounterThread(*this);
    m_timerIsRunning = false;
    m_timerInterval  = 500;
    m_accelTable = LoadAccelerators(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDR_ACCELERATOR1));
    showCounter();
    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestbinDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  } else {
    CDialog::OnSysCommand(nID, lParam);
  }
}

void CTestbinDlg::OnPaint() {
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
        CDialog::OnPaint();
    }
}

HCURSOR CTestbinDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

void CTestbinDlg::OnFileExit() {
  EndDialog(IDOK);
}

void CTestbinDlg::OnOK() {
}

void CTestbinDlg::OnCancel() {
}

void CTestbinDlg::OnClose() {
  OnFileExit();
}

BOOL CTestbinDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return CDialog::PreTranslateMessage(pMsg);
}

void CTestbinDlg::startTimer() {
  if(!m_timerIsRunning && SetTimer(1,m_timerInterval,NULL)) {
    m_timerIsRunning = true;
  }
}

void CTestbinDlg::stopTimer() {
  if(m_timerIsRunning) {
    KillTimer(1);
    m_timerIsRunning = false;
  }
}

static TCHAR *sprintbin(TCHAR *str, UINT64 n) {
  TCHAR *s = str;
  for(int i = 63; i >= 0; i--) {
    *(s++) = ((n >> i) & 1) ? '1':'0';
    if(i % 4 == 0) *(s++) = ' ';
  }
  *s = 0;
  return str;
}

UINT CounterThread::run() {
  for(;;)
    m_counter++;
  return 0;
}

void CTestbinDlg::showCounter() {
  TCHAR tmp1[100],tmp2[256];
  UINT64 n = m_counterThread->getCounter();
  _stprintf(tmp2,_T("%s:%016I64X"), sprintbin(tmp1,n),n);
  GetDlgItem(IDC_STATICTEXT)->SetWindowText(tmp2);
}

void CTestbinDlg::OnStartButton() {
  m_counterThread->resume();
  startTimer();
}

void CTestbinDlg::OnStopbutton() {
  m_counterThread->suspend();
  stopTimer();
  showCounter();
}

void CTestbinDlg::OnStepbutton() {
  m_counterThread->step();
  showCounter();
}

void CTestbinDlg::OnResetbutton() {
  stopTimer();
  m_counterThread->suspend();
  m_counterThread->reset();
  showCounter();
}

void CTestbinDlg::OnTimer(UINT_PTR nIDEvent) {
  showCounter();
  CDialog::OnTimer(nIDEvent);
}

void CTestbinDlg::OnViewTimer() {
  CTimerDlg dlg(m_timerInterval);
  if(dlg.DoModal() == IDOK) {
    m_timerInterval = dlg.m_timerInterval;
    if(m_timerIsRunning) {
      stopTimer();
      startTimer();
    }
  }
}
