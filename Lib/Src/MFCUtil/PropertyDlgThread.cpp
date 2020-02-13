#include "pch.h"
#include <MFCUtil/resource.h>
#include <MFCUtil/PropertyDlgThread.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CPropertyDlgThread, CWinThread)

BEGIN_MESSAGE_MAP(CPropertyDlgThread, CWinThread)
END_MESSAGE_MAP()

CPropertyDlgThread *CPropertyDlgThread::startThread(PropertyDialog *dlg) { // static
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
  if(!m_killed) {
    m_inModalLoop = true;
    intptr_t nResponse = m_dlg->DoModal();
    m_inModalLoop = false;
  }
  PostThreadMessage(WM_QUIT,0,0);
  return TRUE;
}

int CPropertyDlgThread::ExitInstance() {
  SAFEDELETE(m_dlg);
  return __super::ExitInstance();
}

void CPropertyDlgThread::setDialogVisible(bool visible) {
  m_lock.wait();
  try {
    if(m_dlg == NULL) noDialogException(__TFUNCTION__);
    if(!m_setVisibleBusy) {
      m_setVisibleBusy = true;
      if(visible != isDialogVisible1()) {
        if(m_inModalLoop) {
          m_dlg->ShowWindow(visible ? SW_SHOW : SW_HIDE);
        } else {
          ResumeThread();
        }
      }
      while(m_dlg->isVisible() != visible) {
        Sleep(50);
      }
      m_setVisibleBusy = false;
    }
  } catch(...) {
    m_lock.notify();
    throw;
  }
  m_lock.notify();
}

bool CPropertyDlgThread::isDialogVisible() const {
  m_lock.wait();
  const bool result = isDialogVisible1();
  m_lock.notify();
  return result;
}

void CPropertyDlgThread::noDialogException(const TCHAR *method) const {
  throwException(_T("%s:Dialog has terminated"), method);
}

void CPropertyDlgThread::setCurrentDialogProperty(const void *v, size_t size) {
  m_lock.wait();
  try {
    if(m_dlg == NULL) noDialogException(__TFUNCTION__);
    __assume(m_dlg);
    m_dlg->setStartProperty(v, size);
    if(m_dlg->isVisible()) {
      m_dlg->PostMessage(_ID_MSG_RESETCONTROLS);
    }
  } catch(...) {
    m_lock.notify();
    throw;
  }
  m_lock.notify();
}

const void *CPropertyDlgThread::getCurrentDialogProperty(size_t size) const {
  if(m_dlg == NULL) noDialogException(__TFUNCTION__);
  __assume(m_dlg);
  return m_dlg->getCurrentProperty(size);
}

void CPropertyDlgThread::reposition() {
  if(isDialogVisible()) {
    m_dlg->reposition();
  }
}

void CPropertyDlgThread::kill() {
  m_lock.wait();
  if(!m_killed) {
    m_killed = true;
    if(!m_inModalLoop) {
      ResumeThread();
    } else {
      m_dlg->EndDialog(IDOK);
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
  m_lock.notify();
}
