#pragma once

#include <TinyBitSet.h>
#include <PropertyContainer.h>
#include <SafeRunnable.h>
#include <FastSemaphore.h>

using namespace Expr;

typedef enum {
  DBG_RUNNING           // bool
 ,DBG_TERMINATED        // bool
 ,DBG_ERROR             // TCHAR*
} DebuggerProperty;

class Debugger : public SafeRunnable, public PropertyContainer, public PropertyChangeListener {
private:
  bool                  m_running, m_killed, m_terminated;
  FastSemaphore         m_continueSem;
#ifdef TRACE_REDUCTION_CALLSTACK
  ReductionStack       *m_reductionStack;
  UINT                  m_breakOnTopIndex;
#endif

  String                m_errorMsg;
  BitSet8               m_breakPoints;
  bool                  m_stoppedOnReturn;
  Expression           &m_expr;
  const Expression     *m_exprp;
  ParserTree           *m_treep;
  void suspend();
  void resume();
  void stop(bool onReturn = false);
  void throwInvalidStateException(const TCHAR *method, ParserTreeState state) const;
public:
  Debugger(Expression &expr);
  ~Debugger();
  UINT safeRun();

  void go();
  void singleStep();
  void singleSubStep();
#ifdef TRACE_REDUCTION_CALLSTACK
  void goUntilReturn();
  inline const ReductionStack &getReductionStack() const {
    return *m_reductionStack;
  }
#endif /// TRACE_REDUCTION_CALLSTACK

  String getDebugInfo() const;
  void stopASAP();
  void kill();

  Expression &getDebugExpr() {
    return *(Expression*)m_exprp;
  }
  inline ParserTreeState getTreeState() const {
    return m_treep->getState();
  }
  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);

  inline const String &getErrorMsg() const {
    return m_errorMsg;
  }
  inline bool isRunning() const {
    return m_running;
  }
  inline bool isTerminated() const {
    return m_terminated;
  }
};
