#pragma once

#include <Thread.h>
#include <PropertyContainer.h>
#include <TinyBitSet.h>

typedef enum {
  DEBUGGER_CREATED
 ,DEBUGGER_RUNNING
 ,DEBUGGER_BREAK
 ,DEBUGGER_TERMINATED
} DebuggerRunState;

typedef enum {
  DEBUGGER_RUNSTATE           // RunState
 ,DEBUGGER_REQUEST_TERMINATE  // bool*
 ,DEBUGGER_ERROR              // TCHAR*
 ,REMES_PROPERTY              // *RemesPropertyData (oldValue = NULL)
} DebuggerProperty;

class RemesPropertyData {
public:
  const Remes  &m_src;
  RemesProperty m_id;
  const void   *m_oldValue;
  const void   *m_newValue;
  RemesPropertyData(const Remes &src, int id, const void *oldValue, const void *newValue)
    : m_src(src)
    , m_id((RemesProperty)id)
    , m_oldValue(oldValue)
    , m_newValue(newValue)
  {}
};

class Debugger : public Runnable, public PropertyContainer, public PropertyChangeListener {
private:
  DECLARECLASSNAME;
  Remes                        &m_r;
  IntInterval                   m_mInterval, m_kInterval;
  int                           m_maxMKSum;
  bool                          m_skipExisting;
  DebuggerRunState              m_runState;
  bool                          m_requestTerminate;
  FastSemaphore                 m_terminated;
  String                        m_errorMsg;
  BitSet8                       m_breakPoints;
  Thread                       *m_thread;

  void throwInvalidStateException(const TCHAR *method, RemesState state) const;
  void stop();
  void suspend();
  void resume();
public:
  Debugger(Remes &r, const IntInterval &mInterval, const IntInterval &kInterval, int maxMKSum, bool skipExisting);
  ~Debugger();
  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);
  void singleStep();
  void singleSubStep();
  void breakASAP();
  void requestTerminate();
  void go();
  UINT run();
  inline const String &getErrorMsg() const {
    return m_errorMsg;
  }
  inline bool isRunning() const {
    return m_runState == DEBUGGER_RUNNING;
  }
  inline bool isTerminated() const {
    return m_runState == DEBUGGER_TERMINATED;
  }
  inline bool isBreak() const {
    return m_runState == DEBUGGER_BREAK;
  }
         const TCHAR *getStateName() const;
  static const TCHAR *getStateName(DebuggerRunState state);
};
