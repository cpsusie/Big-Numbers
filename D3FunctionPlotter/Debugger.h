#pragma once

#ifdef DEBUG_POLYGONIZER

#ifndef _DEBUG
#error "Must compile with _DEBUG"
#endif

#include <Runnable.h>
#include <PropertyContainer.h>
#include <D3DGraphics/D3SceneEditor.h>
#include "DebugIsoSurface.h"

class CD3FunctionPlotterDlg;

#define BREAKONNEXTFACE  0x01
#define BREAKONNEXTCUBE  0x02
#define BREAKONNEXTLEVEL 0x04
#define BREAKKILLED      0x08

typedef enum {
  DEBUGGER_STATE
} DebuggerProperties;

typedef enum {
  DEBUGGER_CREATED
 ,DEBUGGER_RUNNING
 ,DEBUGGER_PAUSED
 ,DEBUGGER_TERMINATED
} DebuggerState;

class Debugger : public Runnable, public PropertyContainer {
private:
  CD3FunctionPlotterDlg             &m_dlg;
  DebugIsoSurface                   *m_surface;
  String                             m_resultMsg;
  BYTE                               m_breakPoints;
  DebuggerState                      m_state;
  FastSemaphore                      m_terminated, m_go;
  bool                               m_ok;
  void init(bool singleStep);
  void suspend();
  void resume();
public:
  Debugger(CD3FunctionPlotterDlg *dlg);
  ~Debugger();
  UINT run();
  void singleStep();
  void go();
  void goUntilNextCube();
  void breakOnNextLevel(bool on);
  void kill();
  void handleStep(StepType type);
  inline DebuggerState getState() const {
    return m_state;
  }
  inline String getStateName() const {
    return getStateName(getState());
  }
  static String getStateName(DebuggerState state);
  inline bool isOK() const {
    return m_ok;
  }
  D3SceneObject *getSceneObject();
  const DebugIsoSurface &getDebugSurface() const {
    return *m_surface;
  }
};

#endif // DEBUG_POLYGONIZER
