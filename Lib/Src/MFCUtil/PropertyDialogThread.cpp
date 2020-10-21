#include "pch.h"
#include <MFCUtil/resource.h>
#include <MFCUtil/PropertyDialog.h>
#include <MFCUtil/PropertyDialogThread.h>

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CPropertyDialogThread, CWinThread)

BEGIN_MESSAGE_MAP(CPropertyDialogThread, CWinThread)
END_MESSAGE_MAP()

CPropertyDialogThread *CPropertyDialogThread::startThread(AbstractPropertyDialog *dlg) { // static
  CPropertyDialogThread *thr = (CPropertyDialogThread*)AfxBeginThread(RUNTIME_CLASS(CPropertyDialogThread), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
  thr->m_dlg = dlg;
  thr->ResumeThread();
  return thr;
}

CPropertyDialogThread::CPropertyDialogThread() {
  m_bAutoDelete    = true;
  m_setVisibleBusy = false;
  m_inModalLoop    = false;
  m_killed         = false;
}

CPropertyDialogThread::~CPropertyDialogThread() {
}

BOOL CPropertyDialogThread::InitInstance() {
  m_pMainWnd    = m_dlg;
  m_pActiveWnd  = m_dlg;
  SuspendThread(); // next line will be executed when the first call to setActive is done (by ChessDlg)
  if(!m_killed) {
    m_inModalLoop = true;
    intptr_t nResponse = m_dlg->DoModal();
    m_inModalLoop = false;
  }
  PostThreadMessage(WM_QUIT,0,0);
  return TRUE;
}

int CPropertyDialogThread::ExitInstance() {
  SAFEDELETE(m_dlg);
  return __super::ExitInstance();
}

class VisibleChangedNotification : public PropertyChangeListener {
private:
  AbstractPropertyDialog *m_dlg;
  const              bool m_initialVisibleStatus;
  FastSemaphore           m_changed;
public:
  VisibleChangedNotification(AbstractPropertyDialog *dlg)
    : m_dlg(dlg)
    , m_initialVisibleStatus(dlg->isVisible())
    , m_changed(0)
  {
    m_dlg->addPropertyChangeListener(this);
  }
  ~VisibleChangedNotification() {
    m_dlg->removePropertyChangeListener(this);
  }
  void waitUntilChanged() {
    m_changed.wait();
  }
  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);
};

void VisibleChangedNotification::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  if((source == m_dlg) && (m_dlg->getPropertyIdOffset(id) == PROPDLG_VISIBLE_OFFSET)) {
    if(*(bool*)newValue != m_initialVisibleStatus) {
      m_changed.notify();
    }
  }
}

void CPropertyDialogThread::setDialogVisible(bool visible) {
  m_lock.wait();
  try {
    if(m_dlg == nullptr) noDialogException(__TFUNCTION__);
    if(!m_setVisibleBusy) {
      m_setVisibleBusy = true;
      VisibleChangedNotification changed(m_dlg);
      if(visible != isDialogVisible1()) {
        if(m_inModalLoop) {
          m_dlg->ShowWindow(visible ? SW_SHOW : SW_HIDE);
        } else {
          ResumeThread();
        }
        changed.waitUntilChanged();
      }
      m_setVisibleBusy = false;
    }
  } catch(...) {
    m_lock.notify();
    throw;
  }
  m_lock.notify();
}

bool CPropertyDialogThread::isDialogVisible1() const {
  return m_dlg && m_inModalLoop && m_dlg->isVisible();
}

bool CPropertyDialogThread::isDialogVisible() const {
  m_lock.wait();
  const bool result = isDialogVisible1();
  m_lock.notify();
  return result;
}

void CPropertyDialogThread::noDialogException(const TCHAR *method) const {
  throwException(_T("%s:Dialog has terminated"), method);
}

bool CPropertyDialogThread::setCurrentDialogProperty(const void *v, size_t size) {
  m_lock.wait();
  bool changed = false;
  try {
    if(m_dlg == nullptr) noDialogException(__TFUNCTION__);
    __assume(m_dlg);
    changed = m_dlg->setStartProperty(v, size);
    if(m_dlg->isVisible()) {
      m_dlg->PostMessage(_ID_MSG_RESETCONTROLS);
    }
  } catch(...) {
    m_lock.notify();
    throw;
  }
  m_lock.notify();
  return changed;
}

const void *CPropertyDialogThread::getCurrentDialogProperty(size_t size) const {
  if(m_dlg == nullptr) noDialogException(__TFUNCTION__);
  __assume(m_dlg);
  return m_dlg->getCurrentProperty(size);
}

void CPropertyDialogThread::reposition() {
  if(isDialogVisible()) {
    m_dlg->reposition();
  }
}

void CPropertyDialogThread::kill() {
  m_lock.wait();
  if(!m_killed) {
    m_killed = true;
    if(!m_inModalLoop) {
      ResumeThread();
    } else {
      m_dlg->EndDialog(IDOK);
    }

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
  m_lock.notify();
}

PropertyContainer *CPropertyDialogThread::getPropertyContainer() {
  return m_dlg;
}

int CPropertyDialogThread::getPropertyId() const {
  return m_dlg->getPropertyId();
}

String CPropertyDialogThread::getPropertyTypeName() const {
  return m_dlg->getTypeName();
}
