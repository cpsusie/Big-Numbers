#include "stdafx.h"
#include <ThreadPool.h>
#include "Debugger.h"

DEFINECLASSNAME(Debugger);

Debugger::Debugger(Remes &r, const IntInterval &mInterval, const IntInterval &kInterval, int maxMKSum, bool skipExisting)
: m_r(r)
, m_mInterval(   mInterval   )
, m_kInterval(   kInterval   )
, m_maxMKSum(    maxMKSum    )
, m_skipExisting(skipExisting)
, m_runState(DEBUGGER_CREATED)
, m_requestTerminate(false   )
, m_thread(      NULL        )
{
}

Debugger::~Debugger() {
  requestTerminate();
  m_terminated.wait();
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
      m_thread->suspend();
      setProperty(DEBUGGER_RUNSTATE, m_runState, DEBUGGER_RUNNING);
    }
    break;
  }
}

void Debugger::resume() {
  switch(m_runState) {
  case DEBUGGER_CREATED   :
    ThreadPool::executeNoWait(*this);
    break;

  case DEBUGGER_BREAK     :
    m_thread->resume();
    break;
  case DEBUGGER_RUNNING   :
  case DEBUGGER_TERMINATED:
    return; // Do nothing
  }
}

UINT Debugger::run() {
  m_terminated.wait();
  m_thread = Thread::getCurrentThread();
  m_thread->setDescription("DEBUGGER");
  try {
    m_r.addPropertyChangeListener(this);
    try {
      setProperty(DEBUGGER_RUNSTATE, m_runState, DEBUGGER_RUNNING);
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
    } catch(Exception e) {
      if(!m_requestTerminate) {
        setProperty(DEBUGGER_ERROR, m_errorMsg, e.what());
      }
    } catch (...) {
      setProperty(DEBUGGER_ERROR, m_errorMsg, _T("Unknown exception"));
    }
    m_r.removePropertyChangeListener(this);
  } catch(...) {
  }
  m_thread = NULL;
  setProperty(DEBUGGER_RUNSTATE, m_runState, DEBUGGER_TERMINATED);
  m_terminated.notify();
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
  if(!m_breakPoints.isEmpty()) {
    if(m_breakPoints.contains(BREAKASAP)) {
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
          if(m_breakPoints.contains(BREAKSTEP) || m_breakPoints.contains(BREAKSUBSTEP)) {
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
      if(m_breakPoints.contains(BREAKSUBSTEP)) {
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
  case DEBUGGER_CREATED   : return _T("Created");
  case DEBUGGER_RUNNING   : return _T("Running");
  case DEBUGGER_BREAK     : return _T("Suspended");
  case DEBUGGER_TERMINATED: return _T("Terminated");
  }
  return _T("Unknown state");
}

const TCHAR *Debugger::getStateName() const {
  return getStateName(this->m_runState);
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
  case DEBUGGER_CREATED:
  case DEBUGGER_BREAK:
    m_breakPoints.add(   BREAKSTEP   );
    m_breakPoints.remove(BREAKSUBSTEP);
    m_breakPoints.remove(BREAKASAP   );
    resume();
    break;
  case DEBUGGER_RUNNING:
  case DEBUGGER_TERMINATED:
    break;
  }
}

void Debugger::singleSubStep() {
  switch(m_runState) {
  case DEBUGGER_CREATED:
  case DEBUGGER_BREAK:
    m_breakPoints.add(BREAKSTEP   );
    m_breakPoints.add(BREAKSUBSTEP);
    m_breakPoints.remove(BREAKASAP);
    resume();
    break;
  case DEBUGGER_RUNNING:
  case DEBUGGER_TERMINATED:
    break;
  }
}

void Debugger::go() {
  switch(m_runState) {
  case DEBUGGER_CREATED   : 
  case DEBUGGER_BREAK     : 
    m_breakPoints.remove(BREAKSTEP   );
    m_breakPoints.remove(BREAKSUBSTEP);
    m_breakPoints.remove(BREAKASAP   );
    resume();
    break;
  case DEBUGGER_RUNNING   :
  case DEBUGGER_TERMINATED:
    break;;
  }
}

void Debugger::breakASAP() {
  if(!isRunning()) return;
  m_breakPoints.add(   BREAKASAP   );
}
