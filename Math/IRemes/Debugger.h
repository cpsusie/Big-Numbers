#pragma once

#include <InterruptableRunnable.h>
#include <PropertyContainer.h>
#include <FlagTraits.h>

#define FL_BREAKSTEP     0x01
#define FL_BREAKSUBSTEP  0x02
#define FL_ALLBREAKFLAGS (FL_BREAKSTEP | FL_BREAKSUBSTEP)

typedef enum {
  DEBUGGER_STATE           // DebuggerState
 ,REMES_PROPERTY           // *RemesPropertyData (oldValue = NULL)
} DebuggerProperty;

typedef enum {
  DEBUGGER_CREATED
 ,DEBUGGER_RUNNING
 ,DEBUGGER_PAUSED
 ,DEBUGGER_TERMINATED
} DebuggerState;

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

class Debugger : public InterruptableRunnable, public PropertyContainer, public PropertyChangeListener {
private:
  FLAGTRAITS(Debugger, BYTE, m_flags);
  DebuggerState              m_state;
  bool                       m_skipExisting;
  Remes                     &m_r;
  IntInterval                m_mInterval, m_kInterval;
  int                        m_maxMKSum;
  inline Debugger &checkTerminated() {
    if(getState() == DEBUGGER_TERMINATED) throwException(_T("Debugger is terminated"));
    return *this;
  }
  void suspend();
public:
  Debugger(Remes &r, const IntInterval &mInterval, const IntInterval &kInterval, int maxMKSum, bool skipExisting);
  ~Debugger();
  void singleStep(BYTE breakFlags);
  inline void go() {
    singleStep(0);
  }
  void breakASAP();
  void kill();
  UINT safeRun();
  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);
  inline DebuggerState getState() const {
    return m_state;
  }
  String getStateName() const {
    return getStateName(getState());
  }
  static String getStateName(DebuggerState state);
};
