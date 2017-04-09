#pragma once

#include "PropertyDialog.h"

class CPropertyDlgThread : public CWinThread {
private:
  PropertyDialog *m_dlg;
  bool            m_setVisibleBusy : 1;
  bool            m_inModalLoop    : 1;
  bool            m_killed         : 1;

  void noDialogException(const TCHAR *method);
  DECLARE_DYNCREATE(CPropertyDlgThread)
protected:
  CPropertyDlgThread();
  virtual ~CPropertyDlgThread();

  DECLARE_MESSAGE_MAP()
public:
  void setCurrentDialogProperty(const void *v);
  void reposition();
  void setDialogVisible(bool visible);
  bool isDialogVisible() const;
  void kill();
  PropertyContainer *getPropertyContainer() {
    return m_dlg;
  }

  virtual BOOL InitInstance();
  static CPropertyDlgThread *startThread(PropertyDialog *dlg);
};

