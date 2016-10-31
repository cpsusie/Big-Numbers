#include "stdafx.h"
#include "TraceDlgThread.h"
#include "TraceDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CTraceDlgThread, CWinThread)

BEGIN_MESSAGE_MAP(CTraceDlgThread, CWinThread)
END_MESSAGE_MAP()

CTraceDlgThread *CTraceDlgThread::startThread() {
  CTraceDlgThread *traceThread = (CTraceDlgThread*)AfxBeginThread(RUNTIME_CLASS(CTraceDlgThread), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
  traceThread->ResumeThread();
  return traceThread;
}

CTraceDlgThread::CTraceDlgThread() {
  m_bAutoDelete   = true;
  m_setActiveBusy = false;
  m_doModalActive = false;
  m_killed        = false;
  m_active        = false;
}

CTraceDlgThread::~CTraceDlgThread() {
}

BOOL CTraceDlgThread::InitInstance() {
  setSelectedLanguageForThread();
  CTraceDlg dlg(*this);
  m_pMainWnd    = &dlg;
  m_pActiveWnd  = &dlg;
  SuspendThread(); // next line will be executed when the first call to setActive is done (by ChessDlg)
  if(m_killed) {
    return TRUE;
  }
  m_doModalActive = true;
  redirectVerbose(this);
  INT_PTR nResponse =  dlg.DoModal();
  redirectVerbose(NULL);
  return TRUE;
}

void CTraceDlgThread::setActive(bool active) {
  if(!m_setActiveBusy) {
    m_setActiveBusy = true;
    if(active != isActive()) {
      if(m_doModalActive) {
        m_pMainWnd->ShowWindow(active ? SW_SHOW : SW_HIDE);
      } else {
        ResumeThread();
      }
    }
    m_setActiveBusy = false;
  }
}

void CTraceDlgThread::setActiveProperty(bool value) { // called by traceDlg when OnShowWindow is called
  setProperty(TRACEWINDOW_ACTIVE, m_active, value);
}

void CTraceDlgThread::vprintf(const TCHAR *format, va_list argptr) {
  if(isActive()) {
    ((CTraceDlg*)m_pMainWnd)->print(vformat(format, argptr));
  }
}

void CTraceDlgThread::vupdateMessageField(const TCHAR *format, va_list argptr) {
  if(isActive()) {
    ((CTraceDlg*)m_pMainWnd)->updateMessageField(vformat(format, argptr));
  }
}

void CTraceDlgThread::clear() {
  if(isActive()) {
    ((CTraceDlg*)m_pMainWnd)->clear();
  }
}

void CTraceDlgThread::reposition() {
  if(isActive()) {
    ((CTraceDlg*)m_pMainWnd)->reposition();
  }
}

void CTraceDlgThread::kill() {
  m_killed = true;
  PostThreadMessage(WM_QUIT,0,0);
  if(!m_doModalActive) {
    ResumeThread();
  }

  for(;;) {
    DWORD exitCode = 0;
    if(!GetExitCodeThread(m_hThread, &exitCode)) {
      return;
    }
    if(exitCode != STILL_ACTIVE) {
      return;
    }
    Sleep(50);
  }
}
