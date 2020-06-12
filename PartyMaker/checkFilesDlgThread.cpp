#include "stdafx.h"
#include "PartyMaker.h"
#include "CheckFilesDlgThread.h"
#include "CheckFilesDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CCheckFilesDlgThread, CWinThread)

CCheckFilesDlgThread::CCheckFilesDlgThread() {
}

CCheckFilesDlgThread::~CCheckFilesDlgThread() {
}

BOOL CCheckFilesDlgThread::InitInstance() {
  CCheckFilesDlg dlg;
  m_pMainWnd = &dlg;
  m_pActiveWnd = &dlg;

  CWinThread::InitInstance();
  int nResponse = dlg.DoModal();
  return TRUE;
}

int CCheckFilesDlgThread::ExitInstance() {
    return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CCheckFilesDlgThread, CWinThread)
END_MESSAGE_MAP()
