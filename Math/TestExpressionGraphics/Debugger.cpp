#include "stdafx.h"
#include <Thread.h>
#include <Math/Expression/ParserTreeComplexity.h>
#include "Debugger.h"

Debugger::Debugger(Expression &expr)
: m_flags(FL_BREAKSUBSTEP)
, m_state(DEBUGGER_CREATED)
, m_expr(expr)
{
  m_exprp   = &m_expr;
  m_treep   = m_expr.getTree();
  m_treep->addPropertyChangeListener(this);
#ifdef TRACE_REDUCTION_CALLSTACK
  m_reductionStack = &(m_treep->getReductionStack());
  m_reductionStack->addPropertyChangeListener(this);
#endif
}

Debugger::~Debugger() {
  kill();
  m_treep->removePropertyChangeListener(this);
#ifdef TRACE_REDUCTION_CALLSTACK
  m_reductionStack->removePropertyChangeListener(this);
#endif
}

void Debugger::singleStep(BYTE breakFlags) {
  checkTerminated().clrFlag(FL_ALLBREAKFLAGS).setFlag(breakFlags).resume();
}

#ifdef TRACE_REDUCTION_CALLSTACK
void Debugger::goUntilReturn() {
  if(getState() == DEBUGGER_RUNNING) {
    return;
  }
  const ReductionStackElement *e = m_reductionStack->topPointer(isSet(FL_STOPPEDONRETURN)?1:0);
  m_breakOnTopIndex = e ? e->getIndex() : 0;
  singleStep(FL_COUNTONRETURN);
}
#endif // TRACE_REDUCTION_CALLSTACK

void Debugger::stopASAP() {
  if(getState() != DEBUGGER_RUNNING) {
    return;
  }
  setInterrupted();
}

void Debugger::kill() {
  setInterrupted();
  waitUntilJobDone();
  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_TERMINATED);
}

UINT Debugger::safeRun() {
  SETTHREADDESCRIPTION("Debugger");
  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_RUNNING);
  suspend();
  m_treep->reduce();
  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_TERMINATED);
  return 0;
}

void Debugger::suspend() {
  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_PAUSED);
  __super::suspend();
  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_RUNNING);
}

void Debugger::stop(bool onReturn) {
  setFlag(FL_STOPPEDONRETURN, onReturn);
  suspend();
}

#define ISPOP(to,tn) (to && ((tn==NULL) || ((tn)->getIndex() < (to)->getIndex())))
#define ISPOPFROMSAVEDPOP(to,tn) ISPOP(to,tn) && ((to)->getIndex() == m_breakOnTopIndex)

void Debugger::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  if(isInterruptedOrSuspended()) {
    if(isInterrupted()) {
      die();
    } else if (isSuspended()) {
      stop();
      return;
    }
  }

#ifdef TRACE_REDUCTION_CALLSTACK
  if(source == m_reductionStack) {
    switch(id) {
    case REDUCTION_STACKTOP   :
      { const ReductionStackElement *oldTop = (ReductionStackElement*)oldValue;
        const ReductionStackElement *newTop = (ReductionStackElement*)newValue;
        if(isSet(FL_COUNTONRETURN)) {
          if(ISPOPFROMSAVEDPOP(oldTop,newTop)) {
            clrFlag(FL_COUNTONRETURN).setFlag(FL_BREAKONRETURN);
          }
        } else if(isSet(FL_BREAKONRETURN)) {
          if(ISPOPFROMSAVEDPOP(oldTop,newTop)) {
            clrFlag(FL_BREAKONRETURN);
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
        if(isSet(FL_BREAKSUBSTEP)) {
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
        if(isSet(FL_BREAKSTEP)) {
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
      if(isSet(FL_BREAKSUBSTEP)) {
        stop();
      }
      break;

    }
    m_exprp = &m_expr;
  }
}

String Debugger::getFlagNames(BYTE flags) { // static
  const TCHAR *delim = NULL;
  String result;
#define ADDFLAG(f) if(flags & (FL_##f)) { if(delim) result += delim; else delim = _T(" "); result += _T(#f); }
  ADDFLAG(BREAKSTEP      )
  ADDFLAG(BREAKSUBSTEP   )
  ADDFLAG(BREAKONRETURN  )
  ADDFLAG(COUNTONRETURN  )
  ADDFLAG(STOPPEDONRETURN)
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

String Debugger::getDebugInfo() const {
  return format(_T("State:%-14s. it:%d complexity(%s), rat.const:%3zu")
               ,m_treep->getStateName().cstr()
               ,m_treep->getReduceIteration()
               ,ParserTreeComplexity(*m_treep).toString().cstr()
               ,m_treep->getRationalConstantMap().size()
  );
}
