#pragma once

class LoadableMediaArray;

class CProgressDlgThread : public CWinThread {
  DECLARE_DYNCREATE(CProgressDlgThread)
protected:
  CProgressDlgThread();

public:
  LoadableMediaArray *m_mediaArray;
public:

public:
  virtual BOOL InitInstance();
  virtual int ExitInstance();

protected:
  virtual ~CProgressDlgThread();


  DECLARE_MESSAGE_MAP()
};

