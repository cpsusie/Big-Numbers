#include "pch.h"
#include <D3DGraphics/PropertyDlgThread.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CPropertyDlgThread, CWinThread)

BEGIN_MESSAGE_MAP(CPropertyDlgThread, CWinThread)
END_MESSAGE_MAP()

CPropertyDlgThread *CPropertyDlgThread::startThread(PropertyDialog *dlg) {
  CPropertyDlgThread *thr = (CPropertyDlgThread*)AfxBeginThread(RUNTIME_CLASS(CPropertyDlgThread), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
  thr->m_dlg = dlg;
  thr->ResumeThread();
  return thr;
}

CPropertyDlgThread::CPropertyDlgThread() {
  m_bAutoDelete    = true;
  m_setVisibleBusy = false;
  m_inModalLoop    = false;
  m_killed         = false;
}

CPropertyDlgThread::~CPropertyDlgThread() {
}

BOOL CPropertyDlgThread::InitInstance() {
  m_pMainWnd    = m_dlg;
  m_pActiveWnd  = m_dlg;
  SuspendThread(); // next line will be executed when the first call to setActive is done (by ChessDlg)
  if(m_killed) {
    return TRUE;
  }
  m_inModalLoop = true;
  intptr_t nResponse = m_dlg->DoModal();
  m_inModalLoop = false;
  delete m_dlg;
  m_dlg = NULL;
  return TRUE;
}

void CPropertyDlgThread::setDialogVisible(bool visible) {
  DEFINEMETHODNAME;
  if(m_dlg == NULL) noDialogException(method);
  if(!m_setVisibleBusy) {
    m_setVisibleBusy = true;
    if(visible != isDialogVisible()) {
      if(m_inModalLoop) {
        m_dlg->ShowWindow(visible ? SW_SHOW : SW_HIDE);
      } else {
        ResumeThread();
      }
    }
    m_setVisibleBusy = false;
  }
}

bool CPropertyDlgThread::isDialogVisible() const {
  return m_dlg && m_dlg->isVisible();
}

void CPropertyDlgThread::noDialogException(const TCHAR *method) {
  throwException(_T("CPropertyDlgThread::%s:Dialog has terminated"), method);
}

void CPropertyDlgThread::setCurrentDialogProperty(const void *v) {
  DEFINEMETHODNAME;
  if(m_dlg == NULL) noDialogException(method);
  m_dlg->setStartProperty(v);
  if(m_dlg->isVisible()) {
    m_dlg->PostMessage(ID_MSG_RESETCONTROLS);
  }
}

void CPropertyDlgThread::reposition() {
  if(isDialogVisible()) {
    m_dlg->reposition();
  }
}

void CPropertyDlgThread::kill() {
  m_killed = true;
  PostThreadMessage(WM_QUIT,0,0);
  if(!m_inModalLoop) {
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
