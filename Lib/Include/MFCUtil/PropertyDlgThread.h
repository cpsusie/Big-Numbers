#pragma once

#include <FastSemaphore.h>
#include "PropertyDialog.h"

class CPropertyDlgThread : public CWinThread {
private:
  PropertyDialog         *m_dlg;
  bool                    m_setVisibleBusy : 1;
  bool                    m_inModalLoop    : 1;
  bool                    m_killed         : 1;
  mutable FastSemaphore   m_lock;
  void noDialogException(const TCHAR *method) const;
  // no lock-protection
  inline bool isDialogVisible1() const {
    return m_dlg && m_dlg->isVisible();
  }

  DECLARE_DYNCREATE(CPropertyDlgThread)
protected:
  CPropertyDlgThread();
  virtual ~CPropertyDlgThread();

  DECLARE_MESSAGE_MAP()
public:
  void setCurrentDialogProperty(const void *v, size_t size);
  const void *getCurrentDialogProperty(size_t size) const;
  void reposition();
  void setDialogVisible(bool visible);
  bool isDialogVisible() const;
  void kill();
  inline PropertyContainer *getPropertyContainer() {
    return m_dlg;
  }
  inline int getPropertyId() const {
    return m_dlg->getPropertyId();
  }
  virtual BOOL InitInstance();
  virtual int ExitInstance();
  static CPropertyDlgThread *startThread(PropertyDialog *dlg);
};
