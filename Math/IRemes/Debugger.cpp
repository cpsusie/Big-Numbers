#include "stdafx.h"
#include <ThreadPool.h>
#include <Thread.h>
#include <Math/BigReal/BigRealResourcePool.h>
#include "Debugger.h"

DEFINECLASSNAME(Debugger);

Debugger::Debugger(Remes &r, const IntInterval &mInterval, const IntInterval &kInterval, int maxMKSum, bool skipExisting)
: m_breakPoints(     0               )
, m_skipExisting(    skipExisting    )
, m_requestTerminate(false           )
, m_runState(        DEBUGGER_CREATED)
, m_r(               r               )
, m_mInterval(       mInterval       )
, m_kInterval(       kInterval       )
, m_maxMKSum(        maxMKSum        )
{
  ThreadPool::executeNoWait(*this);
}

Debugger::~Debugger() {
  requestTerminate();
}

void Debugger::suspend() {
  switch(m_runState) {
  case DEBUGGER_CREATED   :
  case DEBUGGER_BREAK     :
  case DEBUGGER_TERMINATED:
    return; // Do nothing
  case DEBUGGER_RUNNING   :
    if(!m_requestTerminate) {
      setProperty(DEBUGGER_RUNSTATE, m_runState, DEBUGGER_BREAK);
      __super::suspend();
      setProperty(DEBUGGER_RUNSTATE, m_runState, DEBUGGER_RUNNING);
    }
    break;
  }
}

UINT Debugger::safeRun() {
  setThreadDescription("DEBUGGER");
  setProperty(DEBUGGER_RUNSTATE, m_runState, DEBUGGER_RUNNING);
  try {
    m_r.addPropertyChangeListener(this);
    suspend();
    for(int M = m_mInterval.getFrom(); M <= m_mInterval.getTo(); M++) {
      for(int K = m_kInterval.getFrom(); K <= m_kInterval.getTo(); K++) {
        if(M + K <= m_maxMKSum) {
          if(m_skipExisting && m_r.solutionExist(M, K)) {
            continue;
          }
          m_r.solve(M, K);
        }
      }
    }
    m_r.removePropertyChangeListener(this);
    setProperty(DEBUGGER_RUNSTATE, m_runState, DEBUGGER_TERMINATED);
  } catch (...) {
    m_r.removePropertyChangeListener(this);
    setProperty(DEBUGGER_RUNSTATE, m_runState, DEBUGGER_TERMINATED);
    throw;
  }
  return 0;
}

typedef enum {
  BREAKSTEP
 ,BREAKSUBSTEP
 ,BREAKASAP
} BreakPointType;

void Debugger::throwInvalidStateException(const TCHAR *method, RemesState state) const {
  throwInvalidArgumentException(method, _T("State=%d"), state);
}

void Debugger::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  DEFINEMETHODNAME;
  if(m_requestTerminate) return;

  RemesPropertyData prop(*(const Remes*)source, id, oldValue, newValue);
  notifyPropertyChanged(REMES_PROPERTY, NULL, &prop);
  if(m_breakPoints) {
    if(isSet(BREAKASAP)) {
      stop();
      return;
    }
    switch(id) {
    case REMES_STATE:
      { const RemesState newState = *(RemesState*)prop.m_newValue;
        switch(newState) {
        case REMES_INITIALIZED        :
        case REMES_SOLVE_STARTED      :
        case REMES_SEARCH_COEFFICIENTS:
        case REMES_SEARCH_EXTREMA     :
        case REMES_SUCCEEDED          :
          if(isSet(BREAKSTEP | BREAKSUBSTEP)) {
            stop();
          }
          break;

        default:
          throwInvalidStateException(method, newState);
        }
      }
      break;
    case SEARCHEITERATION     :
    case EXTREMACOUNT         :
    case MAXERROR             :
    case WARNING              :
      if(isSet(BREAKSUBSTEP)) {
        stop();
      }
      break;
    }
  }
}

void Debugger::stop() {
  suspend();
}

const TCHAR *Debugger::getStateName(DebuggerRunState state) { // static
  switch(state) {
  case DEBUGGER_CREATED   : return _T("Created"   );
  case DEBUGGER_RUNNING   : return _T("Running"   );
  case DEBUGGER_BREAK     : return _T("Suspended" );
  case DEBUGGER_TERMINATED: return _T("Terminated");
  }
  return _T("Unknown state");
}

const TCHAR *Debugger::getStateName() const {
  return getStateName(m_runState);
}

void Debugger::requestTerminate() {
  if(m_requestTerminate) return;
  switch(m_runState) {
  case DEBUGGER_CREATED   : 
    setProperty(DEBUGGER_RUNSTATE, m_runState, DEBUGGER_TERMINATED);
    break;
  case DEBUGGER_RUNNING   :
    m_requestTerminate = true;
    BigRealResourcePool::terminateAllPoolCalculations();
    break;
  case DEBUGGER_BREAK     : 
    m_requestTerminate = true;
    BigRealResourcePool::terminateAllPoolCalculations();
    resume();
    break;
  case DEBUGGER_TERMINATED:
    break;
  }
}

void Debugger::singleStep() {
  switch(m_runState) {
  case DEBUGGER_CREATED   :
  case DEBUGGER_BREAK     :
    setFlag(BREAKSTEP).clrFlag(BREAKSUBSTEP | BREAKASAP).resume();
    break;
  }
}

void Debugger::singleSubStep() {
  switch(m_runState) {
  case DEBUGGER_CREATED   :
  case DEBUGGER_BREAK     :
    setFlag(BREAKSTEP | BREAKSUBSTEP).clrFlag(BREAKASAP).resume();
    break;
  }
}

void Debugger::go() {
  switch(m_runState) {
  case DEBUGGER_CREATED   : 
  case DEBUGGER_BREAK     : 
    clrFlag(BREAKSTEP | BREAKSUBSTEP | BREAKASAP ).resume();
    break;
  }
}

void Debugger::breakASAP() {
  if(!isRunning()) return;
  setFlag(BREAKASAP);
}
