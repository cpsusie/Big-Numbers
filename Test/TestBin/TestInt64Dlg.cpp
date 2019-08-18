#include "stdafx.h"
#include "TestBin.h"
#include "TestInt64Dlg.h"
#include "TimerDlg.h"
#include "TestFloatDlg.h"
#include "HexEditOverwrite.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

class CAboutDlg : public CDialog {
public:
  enum { IDD = IDD_ABOUTBOX };
  CAboutDlg() : CDialog(CAboutDlg::IDD) {
  }
protected:
  DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CTestInt64Dlg::CTestInt64Dlg(CWnd *pParent)
: CDialog(CTestInt64Dlg::IDD, pParent)
, m_hexString(EMPTYSTRING)
{
  m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);
}

void CTestInt64Dlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITHEX          , m_hexString            );
  DDV_MaxChars(pDX, m_hexString, 16);
}

BEGIN_MESSAGE_MAP(CTestInt64Dlg, CDialog)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_WM_TIMER()
  ON_WM_CLOSE()
  ON_COMMAND(      ID_FILE_TESTFLOATS    , OnFileTestFloats            )
  ON_COMMAND(      ID_FILE_EXIT          , OnFileExit                  )
  ON_COMMAND(      ID_VIEW_TIMER         , OnViewTimer                 )
  ON_BN_CLICKED(   IDC_STARTBUTTON       , OnStartButton               )
  ON_BN_CLICKED(   IDC_STOPBUTTON        , OnStopButton                )
  ON_BN_CLICKED(   IDC_STEPBUTTON        , OnStepButton                )
  ON_BN_CLICKED(   IDC_RESETBUTTON       , OnResetButton               )
  ON_BN_CLICKED(   IDC_EDITBUTTON        , OnEditButton                )
END_MESSAGE_MAP()

BOOL CTestInt64Dlg::OnInitDialog() {
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

  SetIcon(m_hIcon, TRUE);         // Set big icon
  SetIcon(m_hIcon, FALSE);        // Set small icon

  m_counterThread  = new CounterThread(*this);
  m_timerIsRunning = false;
  m_timerInterval  = 500;
  m_editMode       = false;
  m_accelTable     = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME));
  showCounter();
  ajourEnabling();

  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestInt64Dlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

void CTestInt64Dlg::OnPaint() {
  if(IsIconic()) {
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
    __super::OnPaint();
  }
}

HCURSOR CTestInt64Dlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

void CTestInt64Dlg::OnFileTestFloats() {
  CTestFloatDlg().DoModal();
}

void CTestInt64Dlg::OnFileExit() {
  EndDialog(IDOK);
}

void CTestInt64Dlg::OnOK() {
}

void CTestInt64Dlg::OnCancel() {
}

void CTestInt64Dlg::OnClose() {
  OnFileExit();
}

BOOL CTestInt64Dlg::PreTranslateMessage(MSG *pMsg) {
  if(m_editMode) {
    if(isOverwriteCurrentHexChar(this, pMsg)) {
      UpdateData();
      showCounter(hexStringToInt());
      return true;
    }
  }
  try {
    if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
      return true;
    }
    return __super::PreTranslateMessage(pMsg);
  } catch(Exception e) {
    showWarning(_T("%s"), e.what());
    return true;
  }
}

void CTestInt64Dlg::startTimer() {
  if(!m_timerIsRunning && SetTimer(1,m_timerInterval,NULL)) {
    m_timerIsRunning = true;
  }
}

void CTestInt64Dlg::stopTimer() {
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

void CTestInt64Dlg::showCounter(UINT64 n) {
  TCHAR tmp[100];
  if(m_editMode) {
    setWindowText(this, IDC_STATICTEXT, format(_T("%s:"), sprintbin(tmp, n)));
  } else {
    setWindowText(this, IDC_STATICTEXT, format(_T("%s:%016I64X"), sprintbin(tmp, n), n));
  }
}

void CTestInt64Dlg::ajourEnabling() {
  if(m_editMode) {
    GetDlgItem(IDC_STARTBUTTON)->EnableWindow(FALSE);
    GetDlgItem(IDC_STOPBUTTON )->EnableWindow(FALSE);
    GetDlgItem(IDC_STEPBUTTON )->EnableWindow(FALSE);
    GetDlgItem(IDC_RESETBUTTON)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDITBUTTON )->EnableWindow(TRUE );
  } else if(m_timerIsRunning) {
    GetDlgItem(IDC_STARTBUTTON)->EnableWindow(FALSE);
    GetDlgItem(IDC_STOPBUTTON )->EnableWindow(TRUE );
    GetDlgItem(IDC_STEPBUTTON )->EnableWindow(FALSE);
    GetDlgItem(IDC_RESETBUTTON)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDITBUTTON )->EnableWindow(FALSE);
  } else {
    GetDlgItem(IDC_STARTBUTTON)->EnableWindow(TRUE );
    GetDlgItem(IDC_STOPBUTTON )->EnableWindow(FALSE);
    GetDlgItem(IDC_STEPBUTTON )->EnableWindow(TRUE );
    GetDlgItem(IDC_RESETBUTTON)->EnableWindow(TRUE );
    GetDlgItem(IDC_EDITBUTTON )->EnableWindow(TRUE );
  }
}

void CTestInt64Dlg::OnStartButton() {
  if(m_timerIsRunning) return;
  m_counterThread->resume();
  startTimer();
  ajourEnabling();
}

void CTestInt64Dlg::OnStopButton() {
  if(!m_timerIsRunning) return;
  m_counterThread->suspend();
  stopTimer();
  showCounter();
  ajourEnabling();
}

void CTestInt64Dlg::OnStepButton() {
  if(m_timerIsRunning) {
    OnStopButton();
  }
  m_counterThread->step();
  showCounter();
}

void CTestInt64Dlg::OnResetButton() {
  stopTimer();
  m_counterThread->suspend();
  m_counterThread->reset();
  showCounter();
  ajourEnabling();
}

void CTestInt64Dlg::OnEditButton() {
  if(m_timerIsRunning) return;
  if(m_editMode) {
    leaveEditMode();
    showCounter();
  } else {
    enterEditMode();
  }
  ajourEnabling();
}

void CTestInt64Dlg::enterEditMode() {
  if(m_editMode) return;
  GetDlgItem(IDC_EDITHEX)->ShowWindow(SW_SHOW);
  UINT64 n = m_counterThread->getCounter();
  TCHAR tmp[100];
  _stprintf(tmp, _T("%016I64X"), n);
  m_hexString = tmp;
  UpdateData(FALSE);
  GetDlgItem(IDC_EDITHEX)->SetFocus();
  m_staticTextWinSize = getWindowSize(    this, IDC_STATICTEXT);
  CPoint editPos      = getWindowPosition(this, IDC_EDITHEX   );
  CPoint staticPos    = getWindowPosition(this, IDC_STATICTEXT);
  setWindowSize(this, IDC_STATICTEXT, CSize(editPos.x - staticPos.x - 2, m_staticTextWinSize.cy));
  m_editMode = true;
}

void CTestInt64Dlg::leaveEditMode() {
  if(!m_editMode) return;

  UpdateData();
  m_counterThread->setCounter(hexStringToInt());
  GetDlgItem(IDC_EDITHEX)->ShowWindow(SW_HIDE);
  setWindowSize(this, IDC_STATICTEXT, m_staticTextWinSize);
  m_editMode = false;
}

void CTestInt64Dlg::OnTimer(UINT_PTR nIDEvent) {
  showCounter();
  __super::OnTimer(nIDEvent);
}

void CTestInt64Dlg::OnViewTimer() {
  CTimerDlg dlg(m_timerInterval);
  if(dlg.DoModal() == IDOK) {
    m_timerInterval = dlg.m_timerInterval;
    if(m_timerIsRunning) {
      stopTimer();
      startTimer();
    }
  }
}
