#include "stdafx.h"

#if !defined(TABLEBASE_BUILDER)

#include "MoveFinderExternEngine.h"
#include "EngineOptionsDlgThread.h"
#include "EngineOptionsDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CEngineOptionsDlgThread, CWinThread)

CEngineOptionsDlgThread::CEngineOptionsDlgThread() {
  m_dialogRunning = false;
  m_bAutoDelete   = true;
}

CEngineOptionsDlgThread::~CEngineOptionsDlgThread() {
}

BOOL CEngineOptionsDlgThread::InitInstance() {
  CEngineOptionsDlg dlg(*this);
  m_pMainWnd    = &dlg;
  m_pActiveWnd  = &dlg;
  INT_PTR nResponse =  dlg.DoModal();
  return TRUE;
}

int CEngineOptionsDlgThread::ExitInstance() {
  return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CEngineOptionsDlgThread, CWinThread)
END_MESSAGE_MAP()

void CEngineOptionsDlgThread::setDialogRunning(bool running) {
  setProperty(ENGINEOPIONDIALOG_RUNNING, m_dialogRunning, running);
}

CEngineOptionsDlgThread *CEngineOptionsDlgThread::startThread(MoveFinderExternEngine *moveFinder) { // static
  CEngineOptionsDlgThread *thread = (CEngineOptionsDlgThread*)AfxBeginThread(RUNTIME_CLASS(CEngineOptionsDlgThread), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
  thread->m_engine = &moveFinder->getEngine();
  thread->m_player = moveFinder->getPlayer();
  thread->addPropertyChangeListener(moveFinder);
  thread->ResumeThread();
  return thread;
}

void CEngineOptionsDlgThread::closeThread() {
  PostThreadMessage(WM_QUIT,0,0);
}

#endif
