#include "stdafx.h"
#include <ThreadPool.h>
#include "DebugThread.h"

DEFINECLASSNAME(Debugger);

Debugger::Debugger(Remes &r, const IntInterval &mInterval, const IntInterval &kInterval, int maxMKSum, bool skipExisting)
: m_r(r)
, m_mInterval(   mInterval   )
, m_kInterval(   kInterval   )
, m_maxMKSum(    maxMKSum    )
, m_skipExisting(skipExisting)
, m_running(     false       )
, m_killed(      false       )
, m_terminated(  false       )
, m_thread(      NULL        )
, m_runFinished(0)
{
}

void Debugger::suspend() {
  if(m_terminated) return;
  m_thread->suspend();
}

void Debugger::resume() {
  if(m_terminated) return;
  if(m_thread == NULL) {
    ThreadPool::executeNoWait(*this);
  } else {
    m_thread->resume();
  }
}

UINT Debugger::run() {
  m_thread = Thread::getCurrentThread();
  m_thread->setDescription("DEBUGGER");
  try {
    m_r.addPropertyChangeListener(this);
    try {
      setProperty(THREAD_RUNNING, m_running, true);
      for(int M = m_mInterval.getFrom(); M <= m_mInterval.getTo(); M++) {
        for(int K = m_kInterval.getFrom(); K <= m_kInterval.getTo(); K++) {
          if(M + K <= m_maxMKSum) {
            if(m_skipExisting && m_r.solutionExist(M, K)) {
              continue;
            }
            m_r.solve(M, K);
          }
        }
      }
    } catch(Exception e) {
      setProperty(THREAD_ERROR, m_errorMsg, e.what());
    } catch(bool) {
      // ignore. thrown after resume, when killed
    } catch (...) {
      setProperty(THREAD_ERROR, m_errorMsg, _T("Unknown exception"));
    }
    m_r.removePropertyChangeListener(this);
    setProperty(THREAD_TERMINATED, m_terminated, true );
    setProperty(THREAD_RUNNING   , m_running   , false);
  } catch(...) {
  }
  m_thread = NULL;
  m_runFinished.notify();
  return 0;
}

typedef enum {
  BREAKSTEP
 ,BREAKSUBSTEP
 ,BREAKASAP
} BreakPointType;

void Debugger::throwInvalidStateException(const TCHAR *method, RemesState state) const {
  throwInvalidArgumentException(method, _T("State=%d"), state);
}

void Debugger::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  DEFINEMETHODNAME;
  if(m_killed) throw true;

  RemesPropertyData prop(*(const Remes*)source, id, oldValue, newValue);
  notifyPropertyChanged(REMES_PROPERTY, NULL, &prop);
  if(!m_breakPoints.isEmpty()) {
    if(m_breakPoints.contains(BREAKASAP)) {
      stop();
      return;
    }
    switch(id) {
    case REMES_STATE:
      { const RemesState newState = *(RemesState*)prop.m_newValue;
        switch(newState) {
        case REMES_INITIALIZED        :
        case REMES_SOLVE_STARTED      :
        case REMES_SEARCH_COEFFICIENTS:
        case REMES_SEARCH_EXTREMA     :
        case REMES_SUCCEEDED          :
          if(m_breakPoints.contains(BREAKSTEP) || m_breakPoints.contains(BREAKSUBSTEP)) {
            stop();
          }
          break;

        default:
          throwInvalidStateException(method, newState);
        }
      }
      break;
    case SEARCHEITERATION     :
    case EXTREMACOUNT         :
    case MAXERROR             :
    case WARNING              :
      if(m_breakPoints.contains(BREAKSUBSTEP)) {
        stop();
      }
      break;
    }
  }
}

void Debugger::stop() {
  setProperty(THREAD_RUNNING, m_running, false);
  suspend();
  if(m_killed) throw true;
  setProperty(THREAD_RUNNING, m_running, true);
}

const TCHAR *Debugger::getStateName() const {
  if(m_running)    return _T("Running");
  if(m_terminated) return _T("Terminated");
  if(m_killed)     return _T("Killed");
  return _T("Suspended");
}

void Debugger::kill() {
  if(m_terminated || m_killed) return;
  m_killed = true;
  if(!m_running) {
    resume();
  }
  m_runFinished.wait(4000);
  if(m_thread) {
    throwException(_T("Cannot kill Debugger"));
  }
}

void Debugger::singleStep() {
  if(m_running) return;
  m_breakPoints.add(   BREAKSTEP   );
  m_breakPoints.remove(BREAKSUBSTEP);
  m_breakPoints.remove(BREAKASAP   );
  resume();
}

void Debugger::singleSubStep() {
  if(m_running) return;
  m_breakPoints.add(   BREAKSTEP   );
  m_breakPoints.add(   BREAKSUBSTEP);
  m_breakPoints.remove(BREAKASAP   );
  resume();
}

void Debugger::go() {
  if(m_running) return;
  m_breakPoints.remove(BREAKSTEP   );
  m_breakPoints.remove(BREAKSUBSTEP);
  m_breakPoints.remove(BREAKASAP   );
  resume();
}

void Debugger::stopASAP() {
  if(!m_running) return;
  m_breakPoints.add(   BREAKASAP   );
}
