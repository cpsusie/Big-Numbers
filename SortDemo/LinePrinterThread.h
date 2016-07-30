#pragma once

#include "LinePrinter.h"

class CLinePrinterThread : public CWinThread, public LinePrinter {
private:
  DECLARE_DYNCREATE(CLinePrinterThread)

  bool m_windowTerminated, m_visible;
  friend class CLinePrinterDlg;
  void setVisible(bool visible);
  inline bool isVisible() const {
    return m_visible;
  }
protected:
  CLinePrinterThread();
public:
  void setTitle(const String &title);
  void clear();
  void vprintf(const TCHAR *format, va_list argptr);
  void terminate();
    public:
  virtual BOOL InitInstance();

  static CLinePrinterThread *newThread(PropertyChangeListener *listener);
protected:
  virtual ~CLinePrinterThread();


  DECLARE_MESSAGE_MAP()
};

