#pragma once

class CGameAnalyzeThread : public CWinThread {
    DECLARE_DYNCREATE(CGameAnalyzeThread)
private:
  Semaphore          m_gotParameters;
  const String      *m_name;
  const GameKey     *m_startPosition;
  const GameHistory *m_history;
  int                m_plyIndex;

protected:
    CGameAnalyzeThread();

public:
  void setParameters(const String &name, const GameKey &startPosition, const GameHistory &history, int plyIndex);
  void waitUntilParametersRead();
public:

public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();

protected:
    virtual ~CGameAnalyzeThread();


    DECLARE_MESSAGE_MAP()
};

void startNewGameAnalyzeThread(const String &name, const GameKey &startPosition, const GameHistory &history, int plyIndex);

