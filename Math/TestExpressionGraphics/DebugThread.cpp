#include "stdafx.h"
#include "DebugThread.h"

DebugThread::DebugThread(Expression &expr) : m_expr(expr) {
  m_running = m_killed = m_terminated = false;
  m_exprp   = &m_expr;
  m_treep   = &m_expr;
#ifdef TRACE_REDUCTION_CALLSTACK
  m_reductionStack   = &(m_treep->getReductionStack());
#endif
}

DebugThread::~DebugThread() {
}

typedef enum {
  BREAKSTEP
 ,BREAKSUBSTEP
 ,COUNTONRETURN
 ,BREAKONRETURN
 ,BREAKASAP
} BreakPointType;

void DebugThread::throwInvalidStateException(const TCHAR *method, ParserTreeState state) const {
  throwInvalidArgumentException(method, _T("State=%d"), state);
}

#define ISPOP(to,tn) (to && ((tn==NULL) || ((tn)->getIndex() < (to)->getIndex())))
#define ISPOPFROMSAVEDPOP(to,tn) ISPOP(to,tn) && ((to)->getIndex() == m_breakOnTopIndex)

void DebugThread::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
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
    m_exprp = (const Expression*)source;

    switch(id) {
    case PP_STATE           :
      switch(m_exprp->getState()) {
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

unsigned int DebugThread::run() {
  setDeamon(true);
  try {
    setProperty(THREAD_RUNNING, m_running, true);
    m_expr.reduce();
  } catch(Exception e) {
    m_errorMsg = e.what();
    notifyPropertyChanged(THREAD_ERROR, EMPTYSTRING, m_errorMsg.cstr());
  } catch(bool) {
    // ignore. thrown after resume, when killed
  } catch (...) {
    m_errorMsg = _T("Unknown exception caught in DebugThread");
  }
  setProperty(THREAD_TERMINATED, m_terminated, true );
  setProperty(THREAD_RUNNING   , m_running   , false);

  return 0;
}

void DebugThread::stop(bool onReturn) {
  setProperty(THREAD_RUNNING, m_running, false);
  m_stoppedOnReturn = onReturn;
  suspend();
  if(m_killed) throw true;
  setProperty(THREAD_RUNNING, m_running, true  );
}

void DebugThread::go() {
  if(m_running) return;
  m_breakPoints.clear();
  resume();
}

void DebugThread::singleStep() {
  if(m_running) return;
  m_breakPoints.clear();
  m_breakPoints.add(   BREAKSTEP    );
  resume();
}

void DebugThread::singleSubStep() {
  if(m_running) return;
  m_breakPoints.clear();
  m_breakPoints.add(   BREAKSTEP    );
  m_breakPoints.add(   BREAKSUBSTEP );
  resume();
}

#ifdef TRACE_REDUCTION_CALLSTACK

void DebugThread::goUntilReturn() {
  if(m_running) return;
  const ReductionStackElement *e = m_reductionStack->topPointer(m_stoppedOnReturn?1:0);
  m_breakOnTopIndex = e ? e->getIndex() : 0;

  m_breakPoints.clear();
  m_breakPoints.add(COUNTONRETURN);
  resume();
}
#endif

void DebugThread::stopASAP() {
  if(!m_running) return;
  m_breakPoints.clear();
  m_breakPoints.add(BREAKASAP);
}

void DebugThread::kill() {
  if(!m_terminated) {
    m_killed = true;
    if(m_running) {
      stopASAP();
    } else {
      resume();
    }
  }
  for(int i = 0; i < 10; i++) {
    if(stillActive()) {
      Sleep(50);
    } else {
      break;
    }
  }
  if(stillActive()) {
    throwException(_T("Cannot kill debugThread"));
  }
}
