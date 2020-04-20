#pragma once

#ifdef DEBUG_POLYGONIZER

#ifndef _DEBUG
#error "Must compile with _DEBUG"
#endif

#include <InterruptableRunnable.h>
#include <PropertyContainer.h>
#include <D3DGraphics/D3SceneEditor.h>
#include "DebugIsoSurface.h"

#define FL_BREAKONOCTAINDEX  0x01
#define FL_BREAKONNEXTOCTA   0x02
#define FL_BREAKONNEXTTETRA  0x04
#define FL_BREAKONNEXTFACE   0x08
#define FL_BREAKONNEXTVERTEX 0x10

#define FL_ALLBREAKFLAGS (FL_BREAKONOCTAINDEX | FL_BREAKONNEXTTETRA | FL_BREAKONNEXTFACE | FL_BREAKONNEXTVERTEX)

typedef enum {
  DEBUGGER_STATE
} DebuggerProperties;

typedef enum {
  DEBUGGER_CREATED
 ,DEBUGGER_RUNNING
 ,DEBUGGER_PAUSED
 ,DEBUGGER_TERMINATED
} DebuggerState;

class Debugger : public InterruptableRunnable, public PropertyContainer {
private:
  FLAGTRAITS(Debugger, BYTE, m_flags)
  BitSet                m_octaBreakPoints;
  int                   m_octaIndex;
  DebuggerState         m_state;
  DebugIsoSurface      *m_surface;
  inline Debugger &checkTerminated() {
    if(getState() == DEBUGGER_TERMINATED) throwException(_T("Debugger is terminated"));
    return *this;
  }
  void suspend();
  bool hasOctaBreakPointsAboveCounter(const BitSet &s) const;
public:
  Debugger(D3SceneContainer *sc, const IsoSurfaceParameters &param);
  ~Debugger();
  void singleStep(BYTE breakFlags, const BitSet &octaBreakPoints);
  inline void go(const BitSet &octaBreakPoints) {
    singleStep(FL_BREAKONOCTAINDEX, octaBreakPoints);
  }
  void kill();
  UINT safeRun();
  void handleStep(StepType type);
  inline BYTE getFlags() const {
    return m_flags;
  }
  inline String getFlagNames() const {
    return getFlagNames(getFlags());
  }
  static String getFlagNames(BYTE flags);
  inline DebuggerState getState() const {
    return m_state;
  }
  inline String getStateName() const {
    return getStateName(getState());
  }
  static String getStateName(DebuggerState state);
  D3SceneObjectVisual *getSceneObject();
  const DebugIsoSurface &getDebugSurface() const {
    return *m_surface;
  }
  inline int getOctaIndex() const {
    return m_octaIndex;
  }
};

#endif // DEBUG_POLYGONIZER
