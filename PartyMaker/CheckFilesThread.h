#pragma once

#include <MP3Player.h>

class CCheckFilesThread : public CWinThread {
  DECLARE_DYNCREATE(CCheckFilesThread)
private:
  bool      m_suspend;
protected:
  CCheckFilesThread();
public:
  MP3Player  m_player;
  MediaArray m_mediaArray;
  int        m_index;
  int        m_errorCount;
  void checkNummer();
  void SuspendThread() { m_suspend = true; }
  void ResumeThread()  { m_suspend = false; CWinThread::ResumeThread(); }

public:
  virtual BOOL InitInstance();
  virtual int ExitInstance();
  virtual BOOL OnIdle(LONG lCount);

protected:
  virtual ~CCheckFilesThread();


  DECLARE_MESSAGE_MAP()
};

