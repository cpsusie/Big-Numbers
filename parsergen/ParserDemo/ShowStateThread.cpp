#include "stdafx.h"
#include "ParserDemo.h"
#include "ParserDemoDlg.h"
#include "ShowStateThread.h"
#include "ShowStateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
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
	//{{AFX_MSG_MAP(CShowStateThread)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
