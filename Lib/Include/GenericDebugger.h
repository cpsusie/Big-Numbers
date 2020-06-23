#pragma once

#include <InterruptableRunnable.h>
#include <PropertyContainer.h>

typedef enum {
  DEBUGGER_STATE
} DebuggerProperties;

typedef enum {
  DEBUGGER_CREATED
 ,DEBUGGER_RUNNING
 ,DEBUGGER_PAUSED
 ,DEBUGGER_TERMINATED
} DebuggerState;

class GenericDebugger;

class AbstractDebugableVisualizer : public PropertyChangeListener {
public:
  virtual bool isbreakPoint( GenericDebugger &debugger) = 0;
  virtual void clearAllBreakPoints()                    = 0;
};

class AbstractDebugable : public Runnable {
  friend class GenericDebugger;
protected:
  GenericDebugger *m_debugger;
  virtual const void *getData() const = 0;
};

template<typename T> class Debugable : public AbstractDebugable {
public:
  inline const T &getState() const {
    return *(T*)getData();
  }
};

class GenericDebugger : public InterruptableRunnable, public PropertyContainer {
private:
  DebuggerState                m_state;
  AbstractDebugable           &m_debugable;
  AbstractDebugableVisualizer &m_visualizer;
  bool                         m_breakPointsEnabled;
  GenericDebugger &checkTerminated();
  GenericDebugger &enableBreak(bool enable);
  void suspend();
public:
  GenericDebugger(AbstractDebugable &debugable, AbstractDebugableVisualizer &visualizer);
  ~GenericDebugger() override;
  void singleStep();
  void go();
  void kill();
  UINT safeRun() override;
  void handleStep();
  inline DebuggerState getState() const {
    return m_state;
  }
  inline String getStateName() const {
    return getStateName(getState());
  }
  static String getStateName(DebuggerState state);
  const AbstractDebugable &getDebugable() const {
    return m_debugable;
  }
};
