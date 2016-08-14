#include "stdafx.h"
#include "DebugThread.h"

DEFINECLASSNAME(DebugThread);

DebugThread::DebugThread(int M, int K, Remes &r) : m_M(M), m_K(K), m_r(r) {
  m_running = m_killed = m_terminated = false;
}

DebugThread::~DebugThread() {
}

unsigned int DebugThread::run() {
  setDeamon(true);
  m_r.addPropertyChangeListener(this);
  try {
    setBoolProperty(THREAD_RUNNING, m_running, true);
    m_r.solve(m_M, m_K);
  } catch(Exception e) {
    m_errorMsg = e.what();
    notifyPropertyChanged(THREAD_ERROR, _T(""), m_errorMsg.cstr());
  } catch(bool) {
    // ignore. thrown after resume, when killed
  }
  m_r.removePropertyChangeListener(this);
  setBoolProperty(THREAD_TERMINATED, m_terminated, true );
  setBoolProperty(THREAD_RUNNING   , m_running   , false);
  return 0;
}

typedef enum {
  BREAKSTEP
 ,BREAKSUBSTEP
} BreakPointType;

void DebugThread::throwInvalidStateException(const TCHAR *method, RemesState state) const {
  throwInvalidArgumentException(method, _T("State=%d"), state);
}

void DebugThread::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  DEFINEMETHODNAME;
  if(m_killed) throw true;

  RemesPropertyData prop(*(const Remes*)source, id, oldValue, newValue);
  notifyPropertyChanged(REMES_PROPERTY, NULL, &prop);
  if(!m_breakPoints.isEmpty() && (id == REMES_STATE)) {
    const RemesState newState = *(RemesState*)prop.m_newValue;
    switch(newState) {
    case REMES_INITIALIZE        :
    case REMES_SEARCH_E          :
    case REMES_SEARCH_EXTREMA    :
      if(m_breakPoints.contains(BREAKSUBSTEP)) {
        stop();
      }
      break;

    case REMES_FINALIZE_ITERATION:
    case REMES_NOCONVERGENCE     :
      if(m_breakPoints.contains(BREAKSTEP)) {
        stop();
      }
      break;

    default:
      throwInvalidStateException(method, newState);
    }
  }
}

void DebugThread::stop() {
  setBoolProperty(THREAD_RUNNING, m_running, false);
  suspend();
  if(m_killed) throw true;
  setBoolProperty(THREAD_RUNNING, m_running, true);
}

String DebugThread::getStateName() const {
  if(m_terminated) return _T("Terminated");
  if(m_killed)     return _T("Killed");
  if(m_running)    return _T("Running");
  return _T("Suspended");
}

void DebugThread::kill() {
  if(m_terminated) return;
  m_killed = true;
  if(m_running) {
    stopASAP();
  } else {
    resume();
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

void DebugThread::singleStep() {
  if(m_running) return;
  m_breakPoints.add(   BREAKSTEP   );
  m_breakPoints.remove(BREAKSUBSTEP);
  resume();
}

void DebugThread::singleSubStep() {
  if(m_running) return;
  m_breakPoints.add(   BREAKSTEP   );
  m_breakPoints.add(   BREAKSUBSTEP);
  resume();
}

void DebugThread::go() {
  if(m_running) return;
  m_breakPoints.remove(BREAKSTEP   );
  m_breakPoints.remove(BREAKSUBSTEP);
  resume();
}

void DebugThread::stopASAP() {
  if(!m_running) return;
  m_breakPoints.add(   BREAKSTEP   );
  m_breakPoints.add(   BREAKSUBSTEP);
}
