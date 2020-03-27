#pragma once

#include <InterruptableRunnable.h>
#include <PropertyContainer.h>
#include <FlagTraits.h>

using namespace Expr;

#define FL_BREAKSTEP       0x01
#define FL_BREAKSUBSTEP    0x02
#define FL_BREAKONRETURN   0x04
#define FL_COUNTONRETURN   0x08
#define FL_STOPPEDONRETURN 0x10

#define FL_ALLBREAKFLAGS (FL_BREAKSTEP | FL_BREAKSUBSTEP | FL_BREAKONRETURN | FL_COUNTONRETURN)

typedef enum {
  DEBUGGER_STATE    // DebuggerState
} DebuggerProperties;

typedef enum {
  DEBUGGER_CREATED
 ,DEBUGGER_RUNNING
 ,DEBUGGER_PAUSED
 ,DEBUGGER_TERMINATED
} DebuggerState;

class Debugger : public InterruptableRunnable, public PropertyContainer, public PropertyChangeListener {
private:
  FLAGTRAITS(Debugger, BYTE, m_flags)
  DebuggerState         m_state;
  Expression           &m_expr;
  const Expression     *m_exprp;
  ParserTree           *m_treep;
#ifdef TRACE_REDUCTION_CALLSTACK
  ReductionStack       *m_reductionStack;
  UINT                  m_breakOnTopIndex;
#endif
  inline Debugger &checkTerminated() {
    if(getState() == DEBUGGER_TERMINATED) throwException(_T("Debugger is terminated"));
    return *this;
  }
  void suspend();
  void stop(bool onReturn = false);
public:
  Debugger(Expression &expr);
  ~Debugger();
  void singleStep(BYTE breakFlags);
  inline void go() {
    singleStep(0);
  }
  void stopASAP();
  void kill();
#ifdef TRACE_REDUCTION_CALLSTACK
  void goUntilReturn();
  inline const ReductionStack &getReductionStack() const {
    return *m_reductionStack;
  }
#endif /// TRACE_REDUCTION_CALLSTACK
  UINT safeRun();
  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);
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

  String getDebugInfo() const;

  Expression &getDebugExpr() {
    return *(Expression*)m_exprp;
  }
  inline ParserTreeState getTreeState() const {
    return m_treep->getState();
  }
};
