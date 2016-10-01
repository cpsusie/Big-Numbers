#include "stdafx.h"
#include "OpeningDlgThread.h"
#include "OpeningDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void startNewOpeningDialogThread() {
  COpeningDlgThread *thread = (COpeningDlgThread*)AfxBeginThread(RUNTIME_CLASS(COpeningDlgThread),THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
  thread->ResumeThread();
}

IMPLEMENT_DYNCREATE(COpeningDlgThread, CWinThread)

COpeningDlgThread::COpeningDlgThread() {
  m_bAutoDelete   = true;
}

COpeningDlgThread::~COpeningDlgThread() {
}

//static const LANGID lang[] = { LANG_DANISH, LANG_ENGLISH };

BOOL COpeningDlgThread::InitInstance() {
  setSelectedLanguageForThread();

  COpeningDlg dlg;
  m_pMainWnd    = &dlg;
  m_pActiveWnd  = &dlg;
  dlg.DoModal();

  return TRUE;
}

BEGIN_MESSAGE_MAP(COpeningDlgThread, CWinThread)
END_MESSAGE_MAP()
