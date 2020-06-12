#pragma once

#if !defined(TABLEBASE_BUILDER)

#include <PropertyContainer.h>
#include "Externengine.h"

class MoveFinderExternEngine;

typedef enum {
  ENGINEOPIONDIALOG_RUNNING
} EngineOptionsDlgProperty;

class CEngineOptionsDlgThread : public CWinThread, public PropertyContainer {
private:
  ExternEngine *m_engine;
  Player        m_player;
  bool          m_dialogRunning;
  DECLARE_DYNCREATE(CEngineOptionsDlgThread)
protected:
  CEngineOptionsDlgThread();

public:
  void setDialogRunning(bool running);
  ExternEngine &getEngine() {
    return *m_engine;
  }
  Player getPlayer() const {
    return m_player;
  }

  static CEngineOptionsDlgThread *startThread(MoveFinderExternEngine *moveFinder);
  void closeThread();
public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();

protected:
    virtual ~CEngineOptionsDlgThread();

    DECLARE_MESSAGE_MAP()
};

#endif
