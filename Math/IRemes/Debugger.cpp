#include "stdafx.h"
#include <ThreadPool.h>
#include <Thread.h>
#include <Math/BigReal/BigRealResourcePool.h>
#include "Debugger.h"

Debugger::Debugger(Remes &r, const IntInterval &mInterval, const IntInterval &kInterval, int maxMKSum, bool skipExisting)
: m_flags(        0               )
, m_state(        DEBUGGER_CREATED)
, m_skipExisting( skipExisting    )
, m_r(            r               )
, m_mInterval(    mInterval       )
, m_kInterval(    kInterval       )
, m_maxMKSum(     maxMKSum        )
{
  m_r.addPropertyChangeListener(this);
  ThreadPool::executeNoWait(*this);
}

Debugger::~Debugger() {
  kill();
  m_r.removePropertyChangeListener(this);
}

void Debugger::singleStep(BYTE breakFlags) {
  checkTerminated().clrFlag(FL_ALLBREAKFLAGS).setFlag(breakFlags).resume();
}

void Debugger::kill() {
  setInterrupted();
  BigRealResourcePool::terminateAllPoolCalculations();
  waitUntilJobDone();
  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_TERMINATED);
}

UINT Debugger::safeRun() {
  SETTHREADDESCRIPTION("Debugger");
  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_RUNNING);
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
  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_TERMINATED);
  return 0;
}

void Debugger::suspend() {
  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_PAUSED);
  __super::suspend();
  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_RUNNING);
}

void Debugger::breakASAP() {
  if(getState() == DEBUGGER_RUNNING) {
    setSuspended();
  }
}

void Debugger::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  if(isInterruptedOrSuspended()) {
    if(isInterrupted()) {
      die();
    } else if(isSuspended()) {
      suspend();
      return;
    }
  }

  RemesPropertyData prop(*(const Remes*)source, id, oldValue, newValue);
  notifyPropertyChanged(REMES_PROPERTY, NULL, &prop);
  if(m_flags) {
    switch(id) {
    case REMES_STATE:
      { const RemesState newState = *(RemesState*)prop.m_newValue;
        switch(newState) {
        case REMES_INITIALIZED        :
        case REMES_SOLVE_STARTED      :
        case REMES_SEARCH_COEFFICIENTS:
        case REMES_SEARCH_EXTREMA     :
        case REMES_SUCCEEDED          :
          if(isSet(FL_BREAKSTEP | FL_BREAKSUBSTEP)) {
            suspend();
          }
          break;
        }
      }
      break;
    case SEARCHEITERATION     :
    case EXTREMACOUNT         :
    case MAXERROR             :
    case WARNING              :
      if(isSet(FL_BREAKSUBSTEP)) {
        suspend();
      }
      break;
    }
  }
}

String Debugger::getStateName(DebuggerState state) { // static
#define CASESTR(s) case DEBUGGER_##s: return _T(#s)
  switch(state) {
  CASESTR(CREATED   );
  CASESTR(RUNNING   );
  CASESTR(PAUSED    );
  CASESTR(TERMINATED);
  default: return format(_T("Unknown debuggerState:%d"), state);
  }
#undef CASESTR
}
