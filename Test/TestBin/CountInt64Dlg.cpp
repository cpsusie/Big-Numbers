#include "stdafx.h"
#include "TestBin.h"
#include "CountInt64Dlg.h"
#include "TimerDlg.h"
#include "TestFloatDlg.h"
#include "TestIntDlg.h"
#include "HexEditOverwrite.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

using namespace std;

class CAboutDlg : public CDialog {
public:
  enum { IDD = IDD_ABOUTBOX };
  CAboutDlg() : CDialog(IDD) {
  }
protected:
  DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CCountInt64Dlg::CCountInt64Dlg(StartCommand command, CWnd *pParent)
: CDialog(IDD, pParent)
, m_command(command)
, m_hexString(EMPTYSTRING)
{
  m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);
}

void CCountInt64Dlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(    pDX, IDC_EDIT_HEX, m_hexString    );
  DDV_MaxChars(pDX             , m_hexString, 16);
}

BEGIN_MESSAGE_MAP(CCountInt64Dlg, CDialog)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_WM_TIMER()
  ON_WM_CLOSE()
  ON_COMMAND(      ID_FILE_TESTFLOATS    , OnFileTestFloats            )
  ON_COMMAND(      ID_FILE_TESTINT       , OnFileTestInt               )
  ON_COMMAND(      ID_FILE_EXIT          , OnFileExit                  )
  ON_COMMAND(      ID_VIEW_TIMER         , OnViewTimer                 )
  ON_BN_CLICKED(   IDC_BUTTON_START      , OnButtonStart               )
  ON_BN_CLICKED(   IDC_BUTTON_STOP       , OnButtonStop                )
  ON_BN_CLICKED(   IDC_BUTTON_STEP       , OnButtonStep                )
  ON_BN_CLICKED(   IDC_BUTTON_RESET      , OnButtonReset               )
  ON_BN_CLICKED(   IDC_BUTTON_EDIT       , OnButtonEdit                )
END_MESSAGE_MAP()

BOOL CCountInt64Dlg::OnInitDialog() {
  __super::OnInitDialog();

  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu *pSysMenu = GetSystemMenu(FALSE);
  if(pSysMenu != nullptr) {
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

  switch(m_command) {
  case CMD_TESTFLOAT:
    OnFileTestFloats();
    OnClose();
    break;
  case CMD_TESTINT:
    OnFileTestInt();
    OnClose();
    break;
  }
  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCountInt64Dlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

void CCountInt64Dlg::OnPaint() {
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

HCURSOR CCountInt64Dlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

void CCountInt64Dlg::OnFileTestFloats() {
  CTestFloatDlg().DoModal();
}

void CCountInt64Dlg::OnFileTestInt() {
  CTestIntDlg().DoModal();
}

void CCountInt64Dlg::OnFileExit() {
  EndDialog(IDOK);
}

void CCountInt64Dlg::OnOK() {
}

void CCountInt64Dlg::OnCancel() {
}

void CCountInt64Dlg::OnClose() {
  OnFileExit();
}

BOOL CCountInt64Dlg::PreTranslateMessage(MSG *pMsg) {
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

void CCountInt64Dlg::startTimer() {
  if(!m_timerIsRunning && SetTimer(1,m_timerInterval,nullptr)) {
    m_timerIsRunning = true;
  }
}

void CCountInt64Dlg::stopTimer() {
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

void CCountInt64Dlg::showCounter(UINT64 n) {
  TCHAR tmp[100];
  if(m_editMode) {
    setWindowText(this, IDC_STATIC_TEXT, format(_T("%s:"), sprintbin(tmp, n)));
  } else {
    setWindowText(this, IDC_STATIC_TEXT, format(_T("%s:%016I64X"), sprintbin(tmp, n), n));
  }
}

void CCountInt64Dlg::ajourEnabling() {
  if(m_editMode) {
    GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTON_STOP )->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTON_STEP )->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTON_EDIT )->EnableWindow(TRUE );
  } else if(m_timerIsRunning) {
    GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTON_STOP )->EnableWindow(TRUE );
    GetDlgItem(IDC_BUTTON_STEP )->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTON_EDIT )->EnableWindow(FALSE);
  } else {
    GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE );
    GetDlgItem(IDC_BUTTON_STOP )->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTON_STEP )->EnableWindow(TRUE );
    GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(TRUE );
    GetDlgItem(IDC_BUTTON_EDIT )->EnableWindow(TRUE );
  }
}

void CCountInt64Dlg::OnButtonStart() {
  if(m_timerIsRunning) return;
  m_counterThread->resume();
  startTimer();
  ajourEnabling();
}

void CCountInt64Dlg::OnButtonStop() {
  if(!m_timerIsRunning) return;
  m_counterThread->suspend();
  stopTimer();
  showCounter();
  ajourEnabling();
}

void CCountInt64Dlg::OnButtonStep() {
  if(m_timerIsRunning) {
    OnButtonStop();
  }
  m_counterThread->step();
  showCounter();
}

void CCountInt64Dlg::OnButtonReset() {
  stopTimer();
  m_counterThread->suspend();
  m_counterThread->reset();
  showCounter();
  ajourEnabling();
}

void CCountInt64Dlg::OnButtonEdit() {
  if(m_timerIsRunning) return;
  if(m_editMode) {
    leaveEditMode();
    showCounter();
  } else {
    enterEditMode();
  }
  ajourEnabling();
}

void CCountInt64Dlg::enterEditMode() {
  if(m_editMode) return;
  GetDlgItem(IDC_EDIT_HEX)->ShowWindow(SW_SHOW);
  UINT64 n = m_counterThread->getCounter();
  TCHAR tmp[100];
  _stprintf(tmp, _T("%016I64X"), n);
  m_hexString = tmp;
  UpdateData(FALSE);
  GetDlgItem(IDC_EDIT_HEX)->SetFocus();
  m_staticTextWinSize = getWindowSize(    this, IDC_STATIC_TEXT);
  CPoint editPos      = getWindowPosition(this, IDC_EDIT_HEX   );
  CPoint staticPos    = getWindowPosition(this, IDC_STATIC_TEXT);
  setWindowSize(this, IDC_STATIC_TEXT, CSize(editPos.x - staticPos.x - 2, m_staticTextWinSize.cy));
  m_editMode = true;
}

void CCountInt64Dlg::leaveEditMode() {
  if(!m_editMode) return;

  UpdateData();
  m_counterThread->setCounter(hexStringToInt());
  GetDlgItem(IDC_EDIT_HEX)->ShowWindow(SW_HIDE);
  setWindowSize(this, IDC_STATIC_TEXT, m_staticTextWinSize);
  m_editMode = false;
}

void CCountInt64Dlg::OnTimer(UINT_PTR nIDEvent) {
  showCounter();
  __super::OnTimer(nIDEvent);
}

void CCountInt64Dlg::OnViewTimer() {
  CTimerDlg dlg(m_timerInterval);
  if(dlg.DoModal() == IDOK) {
    m_timerInterval = dlg.m_timerInterval;
    if(m_timerIsRunning) {
      stopTimer();
      startTimer();
    }
  }
}
