#pragma once

class CShowStateThread : public CWinThread {
  DECLARE_DYNCREATE(CShowStateThread)
protected:
  CShowStateThread();

public:
  CDialog *m_maindialog;

  virtual BOOL InitInstance();
  virtual int ExitInstance();

protected:
  virtual ~CShowStateThread();
  DECLARE_MESSAGE_MAP()
};

