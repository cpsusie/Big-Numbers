#include "stdafx.h"

#ifdef DEBUG_POLYGONIZER

#include <Thread.h>
#include "Debugger.h"
#include "DebugIsoSurface.h"
#include "Mainfrm.h"

Debugger::Debugger(D3SceneContainer *sc, const IsoSurfaceParameters &param)
: m_go(0)
, m_surface(NULL)
, m_flags(FL_BREAKONNEXTFACE)
, m_state(DEBUGGER_CREATED)
{
  m_surface = new DebugIsoSurface(this,*sc,param); TRACE_NEW(m_surface);
}

Debugger::~Debugger() {
  kill();
  m_terminated.wait();
  SAFEDELETE(m_surface);
}

void Debugger::singleStep(BYTE breakFlags) {
  checkTerminated().clrFlags(FL_ALLBREAKFLAGS).setFlags(breakFlags).resume();
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
    case NEW_LEVEL:
      if(m_flags & FL_ALLBREAKFLAGS) {
        m_surface->updateSceneObject(MESH_VISIBLE | OCTA_VISIBLE);
        suspend();
      }
      break;
    case NEW_OCTA:
      if(m_flags & FL_BREAKONNEXTOCTA) {
        m_surface->updateSceneObject(MESH_VISIBLE | OCTA_VISIBLE);
        suspend();
      }
      break;
    case NEW_TETRA:
      if(m_flags & FL_BREAKONNEXTTETRA) {
        m_surface->updateSceneObject(MESH_VISIBLE | OCTA_VISIBLE | TETRA_VISIBLE);
        suspend();
      }
      break;
    case NEW_FACE :
      if(m_flags & (FL_BREAKONNEXTFACE | FL_BREAKONNEXTVERTEX)) {
        m_surface->updateSceneObject(MESH_VISIBLE | OCTA_VISIBLE | TETRA_VISIBLE | FACE_VISIBLE | VERTEX_VISIBLE);
        suspend();
      }
      break;
    case NEW_VERTEX:
      if(m_flags & FL_BREAKONNEXTVERTEX) {
        m_surface->updateSceneObject(MESH_VISIBLE | OCTA_VISIBLE | TETRA_VISIBLE | VERTEX_VISIBLE);
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
  ADDFLAG(BREAKONNEXTLEVEL )
  ADDFLAG(BREAKONNEXTOCTA  )
  ADDFLAG(BREAKONNEXTTETRA )
  ADDFLAG(BREAKONNEXTFACE  )
  ADDFLAG(BREAKONNEXTVERTEX)
  ADDFLAG(KILLED           )
  ADDFLAG(ERROR            )
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
