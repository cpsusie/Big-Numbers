#include "stdafx.h"
#include "MessageDlgThread.h"
#include "MessageDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMessageDlgThread, CWinThread)

CMessageDlgThread::CMessageDlgThread() {
}

CMessageDlgThread::~CMessageDlgThread() {
}

BOOL CMessageDlgThread::InitInstance() {
  CMessageDlg dlg(m_milliSeconds, m_caption, m_message);
  m_pActiveWnd = m_pMainWnd = &dlg;

  CWinThread::InitInstance();
  int nResponse = dlg.DoModal();
  return TRUE;
}

BEGIN_MESSAGE_MAP(CMessageDlgThread, CWinThread)
END_MESSAGE_MAP()

void CMessageDlgThread::setParameters(int milliSeconds, const String &caption, const String &message) {
  m_milliSeconds = milliSeconds;
  m_caption      = caption;
  m_message      = message;
  ResumeThread();
}
