#include "stdafx.h"
#include "LinePrinterThread.h"
#include "LinePrinterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CLinePrinterThread, CWinThread)

BEGIN_MESSAGE_MAP(CLinePrinterThread, CWinThread)
END_MESSAGE_MAP()

CLinePrinterThread::CLinePrinterThread() {
  m_bAutoDelete      = true;
  m_windowTerminated = false;
  m_visible          = false;
}

CLinePrinterThread::~CLinePrinterThread() {
  while(!m_windowTerminated) {
    Sleep(30);
  }
}

BOOL CLinePrinterThread::InitInstance() {
  CLinePrinterDlg dlg(*this);
  m_pMainWnd    = &dlg;
  m_pActiveWnd  = &dlg;
  dlg.DoModal();
  const bool oldValue = m_windowTerminated;
  m_windowTerminated = true;
  notifyPropertyChanged(LINEPRINTER_TERMINATED, &oldValue, &m_windowTerminated);
  return TRUE;
}

void CLinePrinterThread::setVisible(bool visible) {
  const bool oldValue = m_visible;
  m_visible = visible;
  if(m_visible != oldValue) {
    notifyPropertyChanged(LINEPRINTER_VISIBLE, &oldValue, &m_visible);
  }
}

void CLinePrinterThread::terminate() {
  if(!m_windowTerminated) {
    m_pMainWnd->SendMessage(WM_CLOSE,0,0);
  }
}

void CLinePrinterThread::vprintf(const TCHAR *format, va_list argptr) {
  if(!m_windowTerminated) {
    ((CLinePrinterDlg*)m_pMainWnd)->addLine(vformat(format,argptr));
  }
}

void CLinePrinterThread::setTitle(const String &title) {
  if(!m_windowTerminated) {
    ((CLinePrinterDlg*)m_pMainWnd)->setTitle(title);
  }
}

void CLinePrinterThread::clear() {
  if(!m_windowTerminated) {
    m_pMainWnd->SendMessage(ID_MSG_CLEARWINDOW);
  }
}

CLinePrinterThread *CLinePrinterThread::newThread(PropertyChangeListener *listener) { // static 
  CLinePrinterThread *thr = (CLinePrinterThread*)AfxBeginThread(RUNTIME_CLASS(CLinePrinterThread),THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
  thr->addPropertyChangeListener(listener);
  thr->ResumeThread();
  while(!thr->isVisible()) {
    Sleep(30);
  }
  return thr;
}
