#include "pch.h"
#include <GenericDebugger.h>
#include <Thread.h>

GenericDebugger::GenericDebugger(AbstractDebugable &debugable, AbstractDebugableVisualizer &visualizer)
: m_state(     DEBUGGER_CREATED)
, m_debugable( debugable       )
, m_visualizer(visualizer      )
{
  addPropertyChangeListener(&m_visualizer);
  m_debugable.m_debugger = this;
}

GenericDebugger::~GenericDebugger() {
  kill();
  m_debugable.m_debugger = NULL;
}

void GenericDebugger::singleStep() {
  checkTerminated().enableBreak(false).resume();
}

void GenericDebugger::go() {
  m_visualizer.clearAllBreakPoints();
  checkTerminated().enableBreak(false).resume();
}

void GenericDebugger::kill() {
  setInterrupted();
  waitUntilJobDone();
  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_TERMINATED);
}

UINT GenericDebugger::safeRun() {
  SETTHREADDESCRIPTION("Debugger");
  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_RUNNING);
  m_debugable.run();
  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_TERMINATED);
  return 0;
}

void GenericDebugger::suspend() {
  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_PAUSED);
  __super::suspend();
  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_RUNNING);
}

void GenericDebugger::handleStep() {
  if(isInterruptedOrSuspended()) {
    if(isInterrupted()) {
      die();
    } else if(isSuspended()) {
      suspend();
      return;
    }
  }
  if(m_breakPointsEnabled && m_visualizer.isbreakPoint(*this)) {
    suspend();
  }
}

String GenericDebugger::getStateName(DebuggerState state) { // static
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

GenericDebugger &GenericDebugger::checkTerminated() {
  if(getState() == DEBUGGER_TERMINATED) {
    throwException(_T("Debugger is terminated"));
  }
  return *this;
}

GenericDebugger &GenericDebugger::enableBreak(bool enable) {
  m_breakPointsEnabled = enable;
  return *this;
}
