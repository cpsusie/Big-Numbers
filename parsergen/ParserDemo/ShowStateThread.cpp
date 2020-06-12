#include "stdafx.h"
#include "ParserDemoDlg.h"
#include "ShowStateThread.h"
#include "ShowStateDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CShowStateThread, CWinThread)

CShowStateThread::CShowStateThread()
{
}

CShowStateThread::~CShowStateThread()
{
}

BOOL CShowStateThread::InitInstance() {
  ShowStateDlg dlg(m_maindialog);
  m_pMainWnd    = &dlg;
  m_pActiveWnd  = &dlg;
  dlg.DoModal();

  return TRUE;
}

int CShowStateThread::ExitInstance() {
  ((CParserDemoDlg*)m_maindialog)->m_showStateThread = NULL;
  return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CShowStateThread, CWinThread)
END_MESSAGE_MAP()
