#include "stdafx.h"

#ifdef DEBUG_POLYGONIZER

#include <Thread.h>
#include "Debugger.h"
#include "DebugIsoSurface.h"
#include "D3FunctionPlotterDlg.h"

#define setBreak(b) m_breakPoints |=  (b)
#define clrBreak(b) m_breakPoints &= ~(b)

Debugger::Debugger(CD3FunctionPlotterDlg *dlg)
: m_dlg(*dlg)
, m_ok(true)
, m_go(0)
, m_surface(NULL)
{
  init(true);
  m_surface = new DebugIsoSurface(this,*dlg,dlg->getIsoSurfaceParameters()); TRACE_NEW(m_surface);
}

void Debugger::init(bool singleStep) {
  m_state              = DEBUGGER_CREATED;
  m_breakPoints        = 0;
  if(singleStep) setBreak(BREAKONNEXTFACE);
  breakOnNextLevel(m_dlg.isBreakOnNextLevelChecked());
}

#define CHECKKILLED()     { if(m_breakPoints & BREAKKILLED) throwException(_T("Killed")); }
#define CHECKTERMINATED() if(getState() == DEBUGGER_TERMINATED) throwException(_T("%s:Debugger is terminated"),__TFUNCTION__)

Debugger::~Debugger() {
  kill();
  m_terminated.wait();
  SAFEDELETE(m_surface);
}

void Debugger::singleStep() {
  CHECKTERMINATED();
  setBreak(BREAKONNEXTFACE);
  resume();
}

void Debugger::go() {
  CHECKTERMINATED();
  clrBreak(BREAKONNEXTFACE|BREAKONNEXTCUBE);
  resume();
}

void Debugger::goUntilNextCube() {
  CHECKTERMINATED();
  clrBreak(BREAKONNEXTFACE);
  setBreak(BREAKONNEXTCUBE);
  resume();
}

void Debugger::breakOnNextLevel(bool on) {
  if(on) {
    setBreak(BREAKONNEXTLEVEL);
  } else {
    clrBreak(BREAKONNEXTLEVEL);
  }
}

void Debugger::kill() {
  if((getState() != DEBUGGER_TERMINATED) && ((m_breakPoints & BREAKKILLED) == 0)) {
    setBreak(BREAKKILLED);
    if(getState() != DEBUGGER_RUNNING) {
      resume();
    }
  }
}

UINT Debugger::run() {
  m_terminated.wait();
  setThreadDescription("Debugger");
  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_RUNNING);
  try {
    suspend();
    m_surface->createData();
  } catch(Exception e) {
    m_resultMsg = e.what();
    m_ok        = false;
  } catch(...) {
    m_resultMsg = _T("Unknown exception");
    m_ok        = false;
  }
  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_TERMINATED);
  m_terminated.notify();
  return 0;
}

void Debugger::suspend() {
  m_surface->updateSceneObject();
  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_PAUSED);
  m_go.wait();
  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_RUNNING);
  CHECKKILLED();
}

void Debugger::resume() {
  m_go.notify();
}

void Debugger::handleStep(StepType type) {
  if(m_breakPoints) {
    CHECKKILLED();
    switch(type) {
    case NEW_FACE :
      if(m_breakPoints & BREAKONNEXTFACE) {
        suspend();
      }
      break;
    case NEW_CUBE :
      if(m_breakPoints & (BREAKONNEXTCUBE|BREAKONNEXTFACE)) {
        suspend();
      }
      break;
    case NEW_LEVEL:
      if(m_breakPoints & (BREAKONNEXTLEVEL|BREAKONNEXTCUBE|BREAKONNEXTFACE)) {
        suspend();
      }
      break;
    }
  }
}

D3SceneObject *Debugger::getSceneObject() {
  return m_surface->getSceneObject();
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

#endif // DEBUG_POLYGONIZER
