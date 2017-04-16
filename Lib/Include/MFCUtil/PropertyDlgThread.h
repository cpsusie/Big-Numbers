#pragma once

#include "PropertyDialog.h"

class CPropertyDlgThread : public CWinThread {
private:
  PropertyDialog *m_dlg;
  bool            m_setVisibleBusy : 1;
  bool            m_inModalLoop    : 1;
  bool            m_killed         : 1;

  void noDialogException(const TCHAR *method) const;
  DECLARE_DYNCREATE(CPropertyDlgThread)
protected:
  CPropertyDlgThread();
  virtual ~CPropertyDlgThread();

  DECLARE_MESSAGE_MAP()
public:
  void setCurrentDialogProperty(const void *v);
  const void *getCurrentDialogProperty() const;
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
  static CPropertyDlgThread *startThread(PropertyDialog *dlg);
};

