#include "stdafx.h"
#include "showgraf.h"
#include "CustomFitThread.h"
#include "CustomFitThreadDlg.h"
#include "DegreeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CCustomFitThread, CWinThread)

CCustomFitThread::CCustomFitThread() {
}

CCustomFitThread::~CCustomFitThread() {
}

BOOL CCustomFitThread::InitInstance() {
  CWinThread::InitInstance();
  m_bAutoDelete = true;
  switch(m_type) {
  case CUSTOM_FIT    :
    { CCustomFitThreadDlg dlg(m_expr, m_range, m_pointArray, *m_fp);
      m_pMainWnd   = &dlg;
      m_pActiveWnd = &dlg;
      dlg.DoModal();
    }
    break;
  case POLYNOMIAL_FIT:
    { CDegreeDlg dlg(m_pointArray, *m_fp);
      m_pMainWnd   = &dlg;
      m_pActiveWnd = &dlg;
      dlg.DoModal();
    }
    break;
  }

  return TRUE;
}

int CCustomFitThread::ExitInstance() {
  return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CCustomFitThread, CWinThread)
    //{{AFX_MSG_MAP(CCustomFitThread)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

CCustomFitThread *startFitThread(FitThreadType type, const DoubleInterval &range, const Point2DArray &pointArray, FunctionPlotter &fp, const CString &expr) {
  CCustomFitThread *thread = (CCustomFitThread*)AfxBeginThread(RUNTIME_CLASS(CCustomFitThread), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
  thread->m_type       = type;
  thread->m_range      = range;
  thread->m_pointArray = pointArray;
  thread->m_fp         = &fp;
  thread->m_expr       = expr;
  thread->ResumeThread();
  return thread;
}
