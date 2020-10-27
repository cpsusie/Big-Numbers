#include "stdafx.h"

#if defined(ISODEBUGGER)

#include <Thread.h>
#include "DebugIsoSurface.h"
#include "Mainfrm.h"
#include "Debugger.h"

Debugger::Debugger(D3SceneContainer *sc, const ExprIsoSurfaceParameters &param)
: m_flags(FL_BREAKONNEXTFACE)
, m_state(DEBUGGER_CREATED)
, m_octaBreakPoints(10000)
, m_octaIndex(-1)
, m_sc(*sc)
{
  m_surface = new DebugIsoSurface(this,*sc,param); TRACE_NEW(m_surface);
}

Debugger::~Debugger() {
  kill();
  SAFEDELETE(m_surface);
}

void Debugger::setState(DebuggerState newState) {
  if(newState != m_state) {
    if(newState == DEBUGGER_RUNNING) {
      m_sc.incrLevel();
    } else if(m_state == DEBUGGER_RUNNING) {
      m_sc.decrLevel();
    }
  }
  setProperty(DEBUGGER_STATE, m_state, newState);
}

bool Debugger::hasOctaBreakPointsAboveCounter(const BitSet &s) const {
  if((intptr_t)s.getCapacity() <= m_octaIndex) {
    return false;
  } else if(m_octaIndex < 0) {
    return !s.isEmpty();
  } else {
    return !BitSet(s).remove(0, m_octaIndex).isEmpty();
  }
}

void Debugger::singleStep(BYTE breakFlags, const BitSet &octaBreakPoints) {
  m_octaBreakPoints = octaBreakPoints;
  if(breakFlags & FL_BREAKONNEXTOCTA) {
    breakFlags &= ~FL_BREAKONNEXTOCTA;
    breakFlags |= FL_BREAKONOCTAINDEX;
    const UINT   nextOctaBreak = m_octaIndex + 1;
    const size_t minCapacity   = nextOctaBreak + 1;
    if(m_octaBreakPoints.getCapacity() < minCapacity) {
      m_octaBreakPoints.setCapacity(2 * minCapacity);
    }
    m_octaBreakPoints.add(nextOctaBreak);
  }
  if(!hasOctaBreakPointsAboveCounter(m_octaBreakPoints)) {
    breakFlags &= ~FL_BREAKONOCTAINDEX;
  }
  checkTerminated().clrFlag(FL_ALLBREAKFLAGS).setFlag(breakFlags).resume();
}

void Debugger::kill() {
  setInterrupted();
  waitUntilJobDone();
  setState(DEBUGGER_TERMINATED);
}

UINT Debugger::safeRun() {
  SETTHREADDESCRIPTION("Debugger");
  setState(DEBUGGER_RUNNING);
  suspend();
  m_surface->createData();
  setState(DEBUGGER_TERMINATED);
  return 0;
}

void Debugger::suspend() {
  setState(DEBUGGER_PAUSED);
  __super::suspend();
  setState(DEBUGGER_RUNNING);
}

void Debugger::handleStep(StepType type) {
  if(isInterruptedOrSuspended()) {
    if(isInterrupted()) {
      die();
    } else if(isSuspended()) {
      suspend();
      return;
    }
  }
  if(m_flags) {
    switch(type) {
    case NEW_OCTA:
      m_octaIndex = m_surface->getCurrentOcta().getCubeIndex();
      if(isSet(FL_BREAKONOCTAINDEX) && m_octaBreakPoints.contains(m_octaIndex)) {
        m_surface->updateSceneObject(MESH_VISIBLE | OCTA_VISIBLE);
        suspend();
      }
      break;
    case NEW_TETRA:
      if(isSet(FL_BREAKONNEXTTETRA)) {
        m_surface->updateSceneObject(MESH_VISIBLE | OCTA_VISIBLE | TETRA_VISIBLE);
        suspend();
      }
      break;
    case NEW_FACE :
      if(isSet(FL_BREAKONNEXTFACE | FL_BREAKONNEXTVERTEX)) {
        m_surface->updateSceneObject(MESH_VISIBLE | OCTA_VISIBLE | TETRA_VISIBLE | FACES_VISIBLE | VERTEX_VISIBLE);
        suspend();
      }
      break;
    case NEW_VERTEX:
      if(isSet(FL_BREAKONNEXTVERTEX)) {
        m_surface->updateSceneObject(MESH_VISIBLE | OCTA_VISIBLE | TETRA_VISIBLE | FACES_VISIBLE | VERTEX_VISIBLE);
        suspend();
      }
      break;
    }
  }
}

D3SceneObjectVisual *Debugger::getSceneObject() {
  return m_surface->getSceneObject();
}

String Debugger::getFlagNames(BYTE flags) { // static
  const TCHAR *delim = nullptr;
  String result;
#define ADDFLAG(f) if(flags & (FL_##f)) { if(delim) result += delim; else delim = _T(" "); result += _T(#f); }
  ADDFLAG(BREAKONOCTAINDEX )
  ADDFLAG(BREAKONNEXTTETRA )
  ADDFLAG(BREAKONNEXTFACE  )
  ADDFLAG(BREAKONNEXTVERTEX)
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

#endif // ISODEBUGGER
