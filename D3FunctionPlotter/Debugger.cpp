#include "stdafx.h"

#ifdef DEBUG_POLYGONIZER

#include <Thread.h>
#include "Debugger.h"
#include "DebugIsoSurface.h"
#include "D3FunctionPlotterDlg.h"

Debugger::Debugger(D3SceneContainer *sc, const IsoSurfaceParameters &param, bool breakOnNextLevel)
: m_go(0)
, m_surface(NULL)
, m_flags(0)
, m_state(DEBUGGER_CREATED)
{
  init(breakOnNextLevel);
  m_surface = new DebugIsoSurface(this,*sc,param); TRACE_NEW(m_surface);
}

void Debugger::init(bool breakOnNextLevel) {
  setFlags(FL_BREAKONNEXTFACE, true).setBreakOnNextLevel(breakOnNextLevel);
}

Debugger::~Debugger() {
  kill();
  m_terminated.wait();
  SAFEDELETE(m_surface);
}

void Debugger::singleStep() {
  checkTerminated().setFlags(FL_BREAKONNEXTFACE).resume();
}

void Debugger::go() {
  checkTerminated().clrFlags(FL_BREAKONNEXTFACE|FL_BREAKONNEXTCUBE).resume();
}

void Debugger::goUntilNextCube() {
  checkTerminated().clrFlags(FL_BREAKONNEXTFACE).setFlags(FL_BREAKONNEXTCUBE).resume();
}

void Debugger::kill() {
  if((getState() != DEBUGGER_TERMINATED) && ((m_flags & FL_KILLED) == 0)) {
    setFlags(FL_KILLED);
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
    m_errorMsg = e.what();
    setFlags(FL_ERROR);
  } catch(...) {
    m_errorMsg = _T("Unknown exception");
    setFlags(FL_ERROR);
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
  checkKilled();
}

void Debugger::resume() {
  m_go.notify();
}

void Debugger::handleStep(StepType type) {
  if(m_flags) {
    checkKilled();
    switch(type) {
    case NEW_FACE :
      if(m_flags & FL_BREAKONNEXTFACE) {
        suspend();
      }
      break;
    case NEW_CUBE :
      if(m_flags & (FL_BREAKONNEXTCUBE|FL_BREAKONNEXTFACE)) {
        suspend();
      }
      break;
    case NEW_LEVEL:
      if(m_flags & (FL_BREAKONNEXTLEVEL|FL_BREAKONNEXTCUBE|FL_BREAKONNEXTFACE)) {
        suspend();
      }
      break;
    }
  }
}

D3SceneObject *Debugger::getSceneObject() {
  return m_surface->getSceneObject();
}

String Debugger::getFlagNames(BYTE flags) { // static
  const TCHAR *delim = NULL;
  String result;
#define ADDFLAG(f) if(flags & (FL_##f)) { if(delim) result += delim; else delim = _T(" "); result += _T(#f); }
  ADDFLAG(BREAKONNEXTFACE )
  ADDFLAG(BREAKONNEXTCUBE )
  ADDFLAG(BREAKONNEXTLEVEL)
  ADDFLAG(KILLED          )
  ADDFLAG(ERROR           )
  return result;
#undef ADDFLAG
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
