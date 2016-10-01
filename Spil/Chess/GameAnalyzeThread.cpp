#include "stdafx.h"
#include "ChessDlg.h"
#include "GameAnalyzeThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CGameAnalyzeThread, CWinThread)

void startNewGameAnalyzeThread(const String &name, const GameKey &startPosition, const GameHistory &history, int plyIndex) {
  CGameAnalyzeThread *thread = (CGameAnalyzeThread*)AfxBeginThread(RUNTIME_CLASS(CGameAnalyzeThread),THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
  thread->setParameters(name, startPosition, history, plyIndex);
  thread->ResumeThread();
  thread->waitUntilParametersRead();
}

CGameAnalyzeThread::CGameAnalyzeThread() : m_gotParameters(0) {
  m_bAutoDelete = true;
}

void CGameAnalyzeThread::setParameters(const String &name, const GameKey &startPosition, const GameHistory &history, int plyIndex) {
  m_name          = &name;
  m_startPosition = &startPosition;
  m_history       = &history;
  m_plyIndex      = plyIndex;
}

void CGameAnalyzeThread::waitUntilParametersRead() {
  m_gotParameters.wait();
}

CGameAnalyzeThread::~CGameAnalyzeThread() {
}

BOOL CGameAnalyzeThread::InitInstance() {
  CChessDlg dlg(*m_name, *m_startPosition, *m_history, m_plyIndex);
  m_pMainWnd    = &dlg;
  m_pActiveWnd  = &dlg;

  m_gotParameters.signal();
  dlg.DoModal();
  return TRUE;
}

int CGameAnalyzeThread::ExitInstance() {
  return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CGameAnalyzeThread, CWinThread)
END_MESSAGE_MAP()
