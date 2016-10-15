#pragma once

class CMessageDlgThread : public CWinThread {
private:
  DECLARE_DYNCREATE(CMessageDlgThread)

  int    m_milliSeconds;
  String m_caption;
  String m_message;

protected:
  CMessageDlgThread();

public:
  virtual BOOL InitInstance();

  void setParameters(int milliSeconds, const String &caption, const String &message);
protected:
  virtual ~CMessageDlgThread();


  DECLARE_MESSAGE_MAP()
};

