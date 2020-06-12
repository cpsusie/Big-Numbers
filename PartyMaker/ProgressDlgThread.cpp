#include "stdafx.h"
#include "PartyMaker.h"
#include "ProgressDlgThread.h"
#include "ProgressDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CProgressDlgThread, CWinThread)

CProgressDlgThread::CProgressDlgThread() {
}

CProgressDlgThread::~CProgressDlgThread() {
}

BOOL CProgressDlgThread::InitInstance() {
  CProgressDlg dlg(m_mediaArray);
  m_pMainWnd    = &dlg;
  m_pActiveWnd  = &dlg;
  INT_PTR nResponse =  dlg.DoModal();

  return TRUE;
}

int CProgressDlgThread::ExitInstance() {
  return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CProgressDlgThread, CWinThread)
END_MESSAGE_MAP()
