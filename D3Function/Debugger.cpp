#include "stdafx.h"

#ifdef DEBUG_POLYGONIZER

#include <Thread.h>
#include "Debugger.h"
#include "DebugIsoSurface.h"
#include "Mainfrm.h"

Debugger::Debugger(D3SceneContainer *sc, const IsoSurfaceParameters &param)
: m_surface(NULL)
, m_flags(FL_BREAKONNEXTFACE)
, m_state(DEBUGGER_CREATED)
{
  m_surface = new DebugIsoSurface(this,*sc,param); TRACE_NEW(m_surface);
}

Debugger::~Debugger() {
  kill();
  waitUntilJobDone();
  SAFEDELETE(m_surface);
}

void Debugger::singleStep(BYTE breakFlags) {
  checkTerminated().clrFlag(FL_ALLBREAKFLAGS).setFlag(breakFlags).resume();
}

void Debugger::kill() {
  if((getState() != DEBUGGER_TERMINATED) && !isSet(FL_KILLED)) {
    setFlag(FL_KILLED);
    if(getState() != DEBUGGER_RUNNING) {
      resume();
    }
  }
}

UINT Debugger::safeRun() {
  setThreadDescription("Debugger");
  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_RUNNING);
  try {
    suspend();
    m_surface->createData();
    setProperty(DEBUGGER_STATE, m_state, DEBUGGER_TERMINATED);
  } catch(...) {
    setProperty(DEBUGGER_STATE, m_state, DEBUGGER_TERMINATED);
  }
  return 0;
}

void Debugger::suspend() {
  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_PAUSED);
  __super::suspend();
  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_RUNNING);
  checkKilled();
}

void Debugger::handleStep(StepType type) {
  if(m_flags) {
    checkKilled();
    switch(type) {
    case NEW_LEVEL:
      if(isSet(FL_ALLBREAKFLAGS)) {
        m_surface->updateSceneObject(MESH_VISIBLE | OCTA_VISIBLE);
        suspend();
      }
      break;
    case NEW_OCTA:
      if(isSet(FL_BREAKONNEXTOCTA)) {
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
  const TCHAR *delim = NULL;
  String result;
#define ADDFLAG(f) if(flags & (FL_##f)) { if(delim) result += delim; else delim = _T(" "); result += _T(#f); }
  ADDFLAG(BREAKONNEXTLEVEL )
  ADDFLAG(BREAKONNEXTOCTA  )
  ADDFLAG(BREAKONNEXTTETRA )
  ADDFLAG(BREAKONNEXTFACE  )
  ADDFLAG(BREAKONNEXTVERTEX)
  ADDFLAG(KILLED           )
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
