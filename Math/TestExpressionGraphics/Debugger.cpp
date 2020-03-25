#include "stdafx.h"
#include <Thread.h>
#include <Math/Expression/ParserTreeComplexity.h>
#include "Debugger.h"

Debugger::Debugger(Expression &expr) : m_expr(expr), m_continueSem(0) {
  m_running = m_killed = m_terminated = false;
  m_exprp   = &m_expr;
  m_treep   = m_expr.getTree();
  m_treep->addPropertyChangeListener(this);
#ifdef TRACE_REDUCTION_CALLSTACK
  m_reductionStack = &(m_treep->getReductionStack());
  m_reductionStack->addPropertyChangeListener(this);
#endif
}

Debugger::~Debugger() {
  m_treep->removePropertyChangeListener(this);
#ifdef TRACE_REDUCTION_CALLSTACK
  m_reductionStack->removePropertyChangeListener(this);
#endif
}

typedef enum {
  BREAKSTEP
 ,BREAKSUBSTEP
 ,COUNTONRETURN
 ,BREAKONRETURN
 ,BREAKASAP
} BreakPointType;

void Debugger::throwInvalidStateException(const TCHAR *method, ParserTreeState state) const {
  throwInvalidArgumentException(method, _T("State=%d"), state);
}

#define ISPOP(to,tn) (to && ((tn==NULL) || ((tn)->getIndex() < (to)->getIndex())))
#define ISPOPFROMSAVEDPOP(to,tn) ISPOP(to,tn) && ((to)->getIndex() == m_breakOnTopIndex)

void Debugger::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  if(m_killed) throw true;

  if(m_breakPoints.contains(BREAKASAP)) {
    stop();
    return;
  }

#ifdef TRACE_REDUCTION_CALLSTACK
  if(source == m_reductionStack) {
    switch(id) {
    case REDUCTION_STACKTOP   :
      { const ReductionStackElement *oldTop = (ReductionStackElement*)oldValue;
        const ReductionStackElement *newTop = (ReductionStackElement*)newValue;
        if(m_breakPoints.contains(COUNTONRETURN)) {
          if(ISPOPFROMSAVEDPOP(oldTop,newTop)) {
            m_breakPoints.remove(COUNTONRETURN);
            m_breakPoints.add(   BREAKONRETURN);
          }
        } else if(m_breakPoints.contains(BREAKONRETURN)) {
          if(ISPOPFROMSAVEDPOP(oldTop,newTop)) {
            m_breakPoints.remove(BREAKONRETURN);
            stop(true);
            break;
          }
        }
      }
      break;
    case REDUCTION_STACKHIGHT :
      { const int oldStack = *(int*)oldValue;
        const int newStack = *(int*)newValue;
        if(newStack > oldStack) { // its a push
          const ReductionStackElement &top = m_reductionStack->top();
          if(top.hasNode() && top.getNode()->isBreakPoint()) {
            stop();
            break;
          }
        }
        if(m_breakPoints.contains(BREAKSUBSTEP)) {
          stop();
          break;
        }
      }
    }
    return;
  }
#endif

  if(source == m_treep) {
    m_exprp = &m_treep->getExpression();

    switch(id) {
    case PP_STATE           :
      switch(m_treep->getState()) {
      case PS_EMPTY    :
      case PS_COMPILED :
      case PS_DERIVED  :
        break;
      default:
        if(m_breakPoints.contains(BREAKSTEP)) {
          stop();
        }
        break;
      }
      break;
    case PP_ROOT            :
      { const ExpressionNode *newRoot = (const ExpressionNode*)newValue;
        if(newRoot) { // dont stop on null-trees
          stop();
        }
      }
      break;

    case PP_REDUCEITERATION :
      if(m_breakPoints.contains(BREAKSUBSTEP)) {
        stop();
      }
      break;

    }
    m_exprp = &m_expr;
  }
}

String Debugger::getDebugInfo() const {
  return format(_T("State:%-14s. it:%d complexity(%s), rat.const:%3zu")
               ,m_treep->getStateName().cstr()
               ,m_treep->getReduceIteration()
               ,ParserTreeComplexity(*m_treep).toString().cstr()
               ,m_treep->getRationalConstantMap().size()
  );
}

UINT Debugger::safeRun() {
  setThreadDescription(_T("Debugger"));
  try {
    suspend();
    setProperty(DBG_RUNNING, m_running, true);
    m_treep->reduce();
  } catch(Exception e) {
    m_errorMsg = e.what();
    notifyPropertyChanged(DBG_ERROR, EMPTYSTRING, m_errorMsg.cstr());
  } catch(bool) {
    // ignore. thrown after resume, when killed
  } catch (...) {
    m_errorMsg = _T("Unknown exception caught in Debugger");
  }
  setProperty(DBG_TERMINATED, m_terminated, true );
  setProperty(DBG_RUNNING   , m_running   , false);
  return 0;
}

void Debugger::suspend() {
  m_continueSem.wait();
}

void Debugger::resume() {
  m_continueSem.notify();
}

void Debugger::stop(bool onReturn) {
  setProperty(DBG_RUNNING, m_running, false);
  m_stoppedOnReturn = onReturn;
  suspend();
  if(m_killed) throw true;
  setProperty(DBG_RUNNING, m_running, true  );
}

void Debugger::go() {
  if(m_running) return;
  m_breakPoints.clear();
  resume();
}

void Debugger::singleStep() {
  if(m_running) return;
  m_breakPoints.clear();
  m_breakPoints.add(   BREAKSTEP    );
  resume();
}

void Debugger::singleSubStep() {
  if(m_running) return;
  m_breakPoints.clear();
  m_breakPoints.add(   BREAKSTEP    );
  m_breakPoints.add(   BREAKSUBSTEP );
  resume();
}

#ifdef TRACE_REDUCTION_CALLSTACK

void Debugger::goUntilReturn() {
  if(m_running) return;
  const ReductionStackElement *e = m_reductionStack->topPointer(m_stoppedOnReturn?1:0);
  m_breakOnTopIndex = e ? e->getIndex() : 0;

  m_breakPoints.clear();
  m_breakPoints.add(COUNTONRETURN);
  resume();
}
#endif

void Debugger::stopASAP() {
  if(!m_running) return;
  m_breakPoints.clear();
  m_breakPoints.add(BREAKASAP);
}

void Debugger::kill() {
  if(!m_terminated) {
    m_killed = true;
    if(m_running) {
      stopASAP();
    } else {
      resume();
    }
  }
  waitUntilJobDone();
}
