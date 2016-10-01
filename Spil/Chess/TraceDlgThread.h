#pragma once

#include <PropertyChangeListener.h>

typedef enum {
  TRACEWINDOW_ACTIVE
} TraceWindowProperties;

class CTraceDlgThread : public CWinThread, public PropertyContainer, VerboseReceiver {
private:
  friend class CTraceDlg;

  bool m_setActiveBusy, m_doModalActive, m_killed; // to prevent infinite recursion
  bool m_active;
  void setActiveProperty(bool value);
  DECLARE_DYNCREATE(CTraceDlgThread)
protected:
  CTraceDlgThread();

public:
  void vprintf(            const TCHAR *format, va_list argptr);
  void vupdateMessageField(const TCHAR *format, va_list argptr);
  void clear();
  void reposition();
  void setActive(bool active);
  bool isActive() const {
    return m_active;
  }
  void kill();
public:
  virtual BOOL InitInstance();

  static CTraceDlgThread *startThread();

protected:
  virtual ~CTraceDlgThread();

  DECLARE_MESSAGE_MAP()
};

