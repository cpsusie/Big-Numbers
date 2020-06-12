#include "stdafx.h"
#include "EditColorMapDlg.h"
#include "EditColorMapDlgThread.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CEditColorMapDlgThread, CWinThread)

BEGIN_MESSAGE_MAP(CEditColorMapDlgThread, CWinThread)
END_MESSAGE_MAP()

CEditColorMapDlgThread *CEditColorMapDlgThread::startThread(PropertyChangeListener *listener, const ColorMapData &cd) {
  CEditColorMapDlgThread *traceThread = (CEditColorMapDlgThread*)AfxBeginThread(RUNTIME_CLASS(CEditColorMapDlgThread), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
  traceThread->addPropertyChangeListener(listener);
  traceThread->m_cd       = cd;
  traceThread->ResumeThread();
  return traceThread;
}

CEditColorMapDlgThread::CEditColorMapDlgThread() {
  m_bAutoDelete   = true;
}

CEditColorMapDlgThread::~CEditColorMapDlgThread() {
}

BOOL CEditColorMapDlgThread::InitInstance() {
  CEditColorMapDlg dlg(this);
  m_pMainWnd    = &dlg;
  m_pActiveWnd  = &dlg;
  m_dlgActive   = true;
  INT_PTR nResponse =  dlg.DoModal();
  setProperty(DIALOGACTIVE, m_dlgActive, false);
  return TRUE;
}

void CEditColorMapDlgThread::setColorMapData(const ColorMapData &cd) {
  setProperty(COLORMAPDATA, m_cd, cd);
}

void CEditColorMapDlgThread::kill() {
  PostThreadMessage(WM_QUIT,0,0);

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
