#include "stdafx.h"
#include <MyUtil.h>
#include <MFCUtil/WinTools.h>
#include <MFCUtil/ProgressDlg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning(disable : 4244)

CProgressDlg::CProgressDlg(CWnd* pParent, Thread &thread, InteractiveRunnable &jobToDo, UINT updateRate) 
: CDialog(CProgressDlg::IDD, pParent)
, m_thread(thread)
, m_jobToDo(jobToDo)
, m_updateRate(updateRate)
, m_supportedFeatures(jobToDo.getSupportedFeatures())
{
  m_accelTable      = NULL;
}

void CProgressDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_BN_CLICKED(_IDC_BUTTONSUSPEND, OnButtonSuspend)
	ON_COMMAND(IDOK, OnOk)
END_MESSAGE_MAP()

BOOL CProgressDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  m_newProgressCtrl.substituteControl(this, _IDC_PROGRESSBAR);
  m_newProgressCtrl.setShowPercent((m_supportedFeatures & IR_SHOWPERCENT) != 0);

  SetWindowText(m_jobToDo.getTitle().cstr());
  CSize winSize = getWindowSize(this);
  if(!(m_supportedFeatures & IR_SHOWPROGRESSMSG)) {
    winSize.cy -= moveControlsBelowUp(getStaticProgressMessage());
    getStaticProgressMessage()->ShowWindow(SW_HIDE);
  }
  if(!(m_supportedFeatures & IR_SHOWTIMEESTIMATE)) {
    winSize.cy -= moveControlsBelowUp(getStaticTimeEstimate());
    getStaticTimeEstimate()->ShowWindow(SW_HIDE);
  } else {
    m_timeElapsedLabel = loadString(_IDS_TIMEELAPSED  );
    m_timeRemaingLabel = loadString(_IDS_TIMEREMAINING);
  }

  if(!(m_supportedFeatures & (IR_PROGRESSBAR | IR_SUBPROGRESSBAR))) {
    winSize.cy -= moveControlsBelowUp(&m_newProgressCtrl);
    m_newProgressCtrl.ShowWindow(SW_HIDE);
  } else {
//    CProgressCtrl *pgsCtrl = getProgressCtrl();
//    CRect wr = getWindowRect(pgsCtrl);
//    pgsCtrl->SetRange(0, m_jobToDo.getMaxProgress());
//    pgsCtrl->SetPos(m_jobToDo.getProgress());
    m_newProgressCtrl.SetRange(0, m_jobToDo.getMaxProgress());
    m_newProgressCtrl.SetPos(m_jobToDo.getProgress());
  }
  if(!(m_supportedFeatures & IR_SUBPROGRESSBAR)) {
    winSize.cy -= moveControlsBelowUp(getSubProgressCtrl());
    getSubProgressCtrl()->ShowWindow(SW_HIDE);
  } else {
    CProgressCtrl *spgsCtrl = getSubProgressCtrl();
    spgsCtrl->SetRange(0, 100);
    spgsCtrl->SetPos(m_jobToDo.getSubProgressPercent());
  }
  int buttonh;
  bool hasButton = false;
  if(!(m_supportedFeatures & IR_INTERRUPTABLE)) {
    buttonh = getWindowSize(getCancelButton()).cy;
    getCancelButton()->ShowWindow(SW_HIDE);
    ModifyStyle(WS_SYSMENU ,0);
    m_accelTable = LoadAccelerators(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(_IDR_PROGRESS_ACCELERATOR));
  } else {
    hasButton = true;
    getCancelButton()->SetWindowText(loadString(_IDC_BUTTONCANCEL).cstr());
  }
  if(!(m_supportedFeatures & IR_SUSPENDABLE)) {
    CRect wr = getWindowRect(getSuspendButton());
    getSuspendButton()->ShowWindow(SW_HIDE);
    if(m_supportedFeatures & IR_INTERRUPTABLE) {
      setWindowRect(getCancelButton(), wr);
    }
    buttonh = wr.Height();
  } else {
    hasButton = true;
    getSuspendButton()->SetWindowText(loadString(_IDC_BUTTONSUSPEND).cstr());
  }
  if(!hasButton) {
    winSize.cy -= buttonh;
  }
  setWindowSize(this, winSize);
  setWaitCursor();
  centerWindow(this);
  startTimer();
  return TRUE;
}

int CProgressDlg::moveControlsBelowUp(CWnd *ctrl) {
  const CRect r = getWindowRect(ctrl);
  const int   h = r.Height();
  static const int ctrlId[6] = { 
    _IDC_PROGRESSBAR 
   ,_IDC_SUBPROGRESSBAR
   ,_IDC_STATICTIMEESTIMATE
   ,_IDC_STATICPROGRESSMSG 
   ,_IDC_BUTTONSUSPEND
   ,IDCANCEL
  };

  for(int i = 0; i < ARRAYSIZE(ctrlId); i++) {
    CWnd *wnd = GetDlgItem(ctrlId[i]);
    CRect r1  = getWindowRect(wnd);
    if(r1.top > r.bottom) {
      r1.top    -= h;
      r1.bottom -= h;
      setWindowRect(wnd, r1);
    }
  }
  return h;
}

void CProgressDlg::OnCancel() {
  m_jobToDo.setInterrupted();
  if(m_jobToDo.isSuspended()) {
    resumeJob();
  }
}

void CProgressDlg::OnButtonSuspend() {
  if(m_jobToDo.isSuspended()) {
    resumeJob();
  } else {
    suspendJob();
  }
}

void CProgressDlg::resumeJob() {
  m_jobToDo.clrSuspended();
  m_thread.resume();
  getSuspendButton()->SetWindowText(loadString(_IDC_BUTTONSUSPEND).cstr());
  startTimer();
}

void CProgressDlg::suspendJob() {
  stopTimer();
  m_jobToDo.setSuspended();
  getSuspendButton()->SetWindowText(loadString(_IDC_BUTTONRESUME).cstr());
}

void CProgressDlg::OnOk() {
  // ignore
}

void CProgressDlg::OnClose() {
  if(!(m_supportedFeatures & IR_INTERRUPTABLE)) {
    return;
  }
  OnCancel();
}

BOOL CProgressDlg::PreTranslateMessage(MSG* pMsg) {
  if(m_accelTable && TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    return true;
  }
  return CDialog::PreTranslateMessage(pMsg);
}

CButton *CProgressDlg::getCancelButton() {
  return (CButton*)GetDlgItem(IDCANCEL);
}

CButton *CProgressDlg::getSuspendButton() {
  return (CButton*)GetDlgItem(_IDC_BUTTONSUSPEND);
}

CStatic *CProgressDlg::getStaticProgressMessage() {
  return (CStatic*)GetDlgItem(_IDC_STATICPROGRESSMSG);
}

CStatic *CProgressDlg::getStaticTimeEstimate() {
  return (CStatic*)GetDlgItem(_IDC_STATICTIMEESTIMATE);
}

/*
CProgressCtrl *CProgressDlg::getProgressCtrl() {
  return (CProgressCtrl*)GetDlgItem(_IDC_PROGRESSBAR);
}
*/

CProgressCtrl *CProgressDlg::getSubProgressCtrl() {
  return (CProgressCtrl*)GetDlgItem(_IDC_SUBPROGRESSBAR);
}

void CProgressDlg::startTimer() {
  SetTimer(1, m_updateRate, NULL);
}

void CProgressDlg::stopTimer() {
  KillTimer(1);
}

static String formatSeconds(double sec) {
  int isec = (int)sec;
  if(sec >= 3600) {
    return format(_T("%d:%02d:%02d"),isec / 3600, (isec % 3600) / 60,isec % 60);
  } else {
    return format(_T("%2d:%02d"),isec / 60,isec % 60);
  }
}

void RollingAverageQueue::add(double n) {
  if((UINT)m_queue.size() == m_maxQueueSize) {
    m_currentSum -= m_queue.get();
  }
  m_queue.put(n);
  m_currentSum += n;
}

void RollingAverageQueue::decrementMaxSize(UINT amount) {
  const UINT newMaxSize = (amount >= m_maxQueueSize) ? 1 : m_maxQueueSize - amount;
  while(m_queue.size() > newMaxSize) {
    m_currentSum -= m_queue.get();
  }
  m_maxQueueSize = newMaxSize;
}

double RollingAverageQueue::getCurrentAverage() const {
  return m_queue.size() ? (m_currentSum / m_queue.size()) : 0;
}

void CProgressDlg::OnTimer( UINT_PTR nIDEvent) {
  if(m_supportedFeatures & (IR_PROGRESSBAR | IR_SUBPROGRESSBAR | IR_SHOWTIMEESTIMATE | IR_SHOWPROGRESSMSG)) {
    short progress   = 0;
    short subPercent = 0;
    if(m_supportedFeatures & (IR_PROGRESSBAR | IR_SUBPROGRESSBAR | IR_SHOWTIMEESTIMATE)) {
      progress = m_jobToDo.getProgress();
      if(m_supportedFeatures & IR_SUBPROGRESSBAR) {
        subPercent = m_jobToDo.getSubProgressPercent();
      }
    }
    if(m_supportedFeatures & (IR_PROGRESSBAR | IR_SUBPROGRESSBAR)) {
//      getProgressCtrl()->SetPos(progress);
      m_newProgressCtrl.SetPos(m_jobToDo.getProgress());

      if(m_supportedFeatures & IR_SUBPROGRESSBAR) {
        getSubProgressCtrl()->SetPos(subPercent);
      }
    }
    if(m_supportedFeatures & IR_SHOWTIMEESTIMATE) {
      const int secondsElapsed = diff(m_jobToDo.getJobStartTime(), Timestamp(), TSECOND);
      const int secondsLeft    = m_jobToDo.getEstimatedSecondsLeft();
      if(secondsLeft < 6) {
        if(m_rollingAverage.getMaxSize() > 5) {
          m_rollingAverage.decrementMaxSize(1);
        }
      }
      m_rollingAverage.add(secondsLeft);
      String timeElapsedMsg   = format(_T("%s:%s"), m_timeElapsedLabel.cstr(), formatSeconds(secondsElapsed).cstr());
      String timeRemainingMsg;
      if(m_rollingAverage.isFull() || (m_rollingAverage.getCurrentSize() >= 5)) {
        timeRemainingMsg = format(_T("%s:%s"), m_timeRemaingLabel.cstr(), formatSeconds(m_rollingAverage.getCurrentAverage()).cstr());
      }
      setWindowText(getStaticTimeEstimate(), format(_T("%s       %s"), timeElapsedMsg.cstr(), timeRemainingMsg.cstr()));
    }
    if(m_supportedFeatures & IR_SHOWPROGRESSMSG) {
      const String msg = m_jobToDo.getProgressMessage();
      getStaticProgressMessage()->SetWindowText(msg.cstr());
    }
  }
  if(!m_thread.stillActive()) {
    setWaitCursor(false);
    OnOK();
  }
  CDialog::OnTimer(nIDEvent);
}

static void setControlCursor(CWnd *wnd, TCHAR *cursorId) {
  setWindowCursor(wnd, cursorId);
}

void CProgressDlg::setWaitCursor(bool on) {
  TCHAR *cursorId = on ? IDC_WAIT : IDC_ARROW;
  setWindowCursor(this                       , cursorId);
  setControlCursor(getCancelButton()         , IDC_ARROW);
  setControlCursor(getSuspendButton()        , IDC_ARROW);
  setControlCursor(getStaticProgressMessage(), cursorId);
  setControlCursor(getStaticTimeEstimate()   , cursorId);
  setControlCursor(&m_newProgressCtrl        , cursorId);
  setControlCursor(getSubProgressCtrl()      , cursorId);
}
