#include "pch.h"
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
, m_timerRunning(false)
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

  setWindowText(this, m_jobToDo.getTitle());
  CSize winSize = getWindowSize(this);
  m_jobCount = minMax(m_jobToDo.getJobCount(), (USHORT)1, (USHORT)4);
  if (!(m_supportedFeatures & (IR_SHOWPROGRESSMSG | IR_SUBPROGRESSBAR))) {
    winSize.cy -= setVisibleJobs(0);
  } else {
    winSize.cy -= setVisibleJobs(m_jobCount);
  }
  if (!(m_supportedFeatures & IR_SHOWTIMEESTIMATE)) {
    winSize.cy -= moveControlsBelowUp(getStaticTimeEstimate());
    getStaticTimeEstimate()->ShowWindow(SW_HIDE);
  } else {
    m_timeElapsedLabel = loadString(_IDS_TIMEELAPSED);
    m_timeRemaingLabel = loadString(_IDS_TIMEREMAINING);
  }

  if (!(m_supportedFeatures & (IR_PROGRESSBAR | IR_SUBPROGRESSBAR))) {
    winSize.cy -= moveControlsBelowUp(&m_newProgressCtrl);
    m_newProgressCtrl.ShowWindow(SW_HIDE);
  } else {
    m_newProgressCtrl.SetRange(0, 1000);
    m_newProgressCtrl.SetPos((int)(m_jobToDo.getPercentDone()*10));
  }
  int buttonh;
  bool hasButton = false;
  if (m_supportedFeatures & IR_INTERRUPTABLE) {
    hasButton = true;
    setWindowText(getCancelButton(), loadString(_IDC_BUTTONCANCEL));
  } else {
    buttonh = getWindowSize(getCancelButton()).cy;
    getCancelButton()->ShowWindow(SW_HIDE);
    ModifyStyle(WS_SYSMENU, 0);
    m_accelTable = LoadAccelerators(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(_IDR_PROGRESS_ACCELERATOR));
  }
  if (m_supportedFeatures & IR_SUSPENDABLE) {
    hasButton = true;
    setWindowText(getSuspendButton(), loadString(_IDC_BUTTONSUSPEND));
  } else {
    CRect wr = getWindowRect(getSuspendButton());
    buttonh = wr.Height();
    getSuspendButton()->ShowWindow(SW_HIDE);
    if (m_supportedFeatures & IR_INTERRUPTABLE) {
      setWindowPosition(getCancelButton(), wr.TopLeft());
    }
  }
  if (!hasButton) {
    winSize.cy -= buttonh;
  }
  setWindowSize(this, winSize);
  setWaitCursor();
  centerWindow(this);
  startTimer();
  return TRUE;
}

typedef struct {
  int m_msgId, m_barId;
} SubProgressBarMsgId;

static const SubProgressBarMsgId barMsgIdArray[] = {
  _IDC_STATICPROGRESSMSG1, _IDC_SUBPROGRESSBAR1
 ,_IDC_STATICPROGRESSMSG2, _IDC_SUBPROGRESSBAR2
 ,_IDC_STATICPROGRESSMSG3, _IDC_SUBPROGRESSBAR3
 ,_IDC_STATICPROGRESSMSG4, _IDC_SUBPROGRESSBAR4
};

int CProgressDlg::setVisibleJobs(UINT count) {
  const int  lh = getWindowPosition(this, _IDC_STATICPROGRESSMSG2).y
                - getWindowPosition(this, _IDC_STATICPROGRESSMSG1).y;

  const UINT maxCount = ARRAYSIZE(barMsgIdArray);
  const int  features = m_supportedFeatures & (IR_SUBPROGRESSBAR|IR_SHOWPROGRESSMSG);
  int        deltah   = 0;
  if (count >= maxCount) count = maxCount;

  if (features & IR_SUBPROGRESSBAR) {
    for (UINT i = 0; i < count; i++) {
      CProgressCtrl *ctrl = getSubProgressCtrl(i);
      ctrl->SetRange(0, 100);
      ctrl->SetPos(m_jobToDo.getSubProgressPercent(i));
    }
  }
  for(UINT i = maxCount; i--;) {
    CWnd *barWin       = getSubProgressCtrl(i);
    CWnd *msgWin       = getStaticProgressMessage(i);
    CRect barr         = getWindowRect(barWin);
    CRect msgr         = getWindowRect(msgWin);
    const bool showbar = (i < count) && (features & IR_SUBPROGRESSBAR);
    const bool showmsg = (i < count) && (features & IR_SHOWPROGRESSMSG);
    if(i < count) {
      if(showbar && !showmsg) {
        barr.left  = msgr.left;
        setWindowRect(barWin,barr);
      } else if(!showbar && showmsg) {
        msgr.right = barr.right;
        setWindowRect(msgWin, msgr);
      }
    }
    if(!showbar && !showmsg) {
      deltah += lh;
      moveControlsBelowUp(msgWin, lh);
    }
    if(!showbar) barWin->ShowWindow(SW_HIDE);
    if(!showmsg) msgWin->ShowWindow(SW_HIDE);
  }
  return deltah;
}

static const int ctrlId[] = {
  _IDC_STATICTIMEESTIMATE
 ,_IDC_PROGRESSBAR       
 ,_IDC_BUTTONSUSPEND     
 ,IDCANCEL               
};

int CProgressDlg::moveControlsBelowUp(CWnd *win, int dh) {
  const CRect r = getWindowRect(win);
  if(dh == 0) {
    CompactIntArray dyarray(ARRAYSIZE(ctrlId));
    for(int i = 0; i < ARRAYSIZE(ctrlId); i++) {
      const int y = getWindowPosition(this, ctrlId[i]).y;
      if(y > r.bottom) dyarray.add(y-r.top);
    }
    if(dyarray.size() == 0) {
      return r.Height();
    }
    dyarray.sort(intHashCmp);
    dh = dyarray[0];
  }
  if(dh == 0) return 0;

  for(int i = 0; i < ARRAYSIZE(ctrlId); i++) {
    CWnd  *ctrl = GetDlgItem(ctrlId[i]);
    CPoint p    = getWindowPosition(ctrl);
    if(p.y > r.bottom) {
      p.y -= dh;
      setWindowPosition(ctrl, p);
    }
  }
  return dh;
}

void CProgressDlg::OnCancel() {
  m_jobToDo.setInterrupted();
  startTimer();
}

void CProgressDlg::OnButtonSuspend() {
  if(m_jobToDo.isSuspended()) {
    resumeJob();
  } else {
    suspendJob();
  }
}

void CProgressDlg::resumeJob() {
  setWindowText(getSuspendButton(), loadString(_IDC_BUTTONSUSPEND));
  m_jobToDo.resume();
  startTimer();
}

void CProgressDlg::suspendJob() {
  stopTimer();
  m_jobToDo.setSuspended();
  setWindowText(getSuspendButton(), loadString(_IDC_BUTTONRESUME));
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

CStatic *CProgressDlg::getStaticProgressMessage(UINT index) {
  return (CStatic*)GetDlgItem(barMsgIdArray[index].m_msgId);
}

CProgressCtrl *CProgressDlg::getSubProgressCtrl(UINT index) {
  return (CProgressCtrl*)GetDlgItem(barMsgIdArray[index].m_barId);
}

CStatic *CProgressDlg::getStaticTimeEstimate() {
  return (CStatic*)GetDlgItem(_IDC_STATICTIMEESTIMATE);
}

void CProgressDlg::startTimer() {
  if(!m_timerRunning) {
    if(SetTimer(1, m_updateRate, NULL) == 1) {
      m_timerRunning = true;
    }
  }
}

void CProgressDlg::stopTimer() {
  if(m_timerRunning) {
    KillTimer(1);
    m_timerRunning = false;
  }
}

static String formatSeconds(double sec) {
  int isec = (int)sec;
  if(sec >= 3600) {
    return format(_T("%d:%02d:%02d"),isec / 3600, (isec % 3600) / 60,isec % 60);
  } else {
    return format(_T("%2d:%02d"),isec / 60,isec % 60);
  }
}

void CProgressDlg::OnTimer( UINT_PTR nIDEvent) {
  if(m_supportedFeatures & (IR_PROGRESSBAR | IR_SUBPROGRESSBAR | IR_SHOWTIMEESTIMATE | IR_SHOWPROGRESSMSG)) {
    CompactShortArray subPercent;
    if(m_supportedFeatures & (IR_PROGRESSBAR | IR_SUBPROGRESSBAR | IR_SHOWTIMEESTIMATE)) {
      if(m_supportedFeatures & IR_SUBPROGRESSBAR) {
        for(UINT i = 0; i < m_jobCount; i++) {
          subPercent.add(m_jobToDo.getSubProgressPercent(i));
        }
      }
    }
    if(m_supportedFeatures & (IR_PROGRESSBAR | IR_SUBPROGRESSBAR)) {
      double promilleDone = m_jobToDo.getPercentDone() * 10;

      if(m_supportedFeatures & IR_SUBPROGRESSBAR) {
        double sumSubPercent = 0;
        for(UINT i = 0; i < subPercent.size(); i++) {
          const SHORT ss = subPercent[i];
          getSubProgressCtrl(i)->SetPos(ss);
          sumSubPercent += ss;
        }
        if(m_supportedFeatures & IR_AUTOCORRELATETIME) {
          if(m_jobCount) promilleDone += sumSubPercent/10.0/m_jobCount;
        }
      }
      m_newProgressCtrl.SetPos((int)promilleDone);
    }
    if(m_supportedFeatures & IR_SHOWTIMEESTIMATE) {
      const double secondsElapsed   = diff(m_jobToDo.getJobStartTime(), Timestamp(), TSECOND);
      const double secondsLeft      = m_jobToDo.getSecondsRemaining();
      const String timeElapsedMsg   = format(_T("%s:%s"), m_timeElapsedLabel.cstr(), formatSeconds(secondsElapsed).cstr());
      const String timeRemainingMsg = format(_T("%s:%s"), m_timeRemaingLabel.cstr(), formatSeconds(secondsLeft).cstr());
      setWindowText(getStaticTimeEstimate(), format(_T("%s       %s"), timeElapsedMsg.cstr(), timeRemainingMsg.cstr()));
    }
    if(m_supportedFeatures & IR_SHOWPROGRESSMSG) {
      for(USHORT i = 0; i < m_jobCount; i++) {
        setWindowText(getStaticProgressMessage(i), m_jobToDo.getProgressMessage(i));
      }
    }
  }
  if(!m_thread.stillActive()) {
    setWaitCursor(false);
    EndDialog(IDOK);
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
  setControlCursor(getStaticTimeEstimate()   , cursorId);
  setControlCursor(&m_newProgressCtrl        , cursorId);
  for(USHORT i = 0; i < m_jobCount; i++) {
    setControlCursor(getStaticProgressMessage(i), cursorId);
    setControlCursor(getSubProgressCtrl(i)      , cursorId);
  }
}
