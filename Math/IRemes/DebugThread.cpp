#include "stdafx.h"
#include "DebugThread.h"

DEFINECLASSNAME(DebugThread);

DebugThread::DebugThread(int M, int K, Remes &r) : m_M(M), m_K(K), m_r(r) {
  m_running = m_killed = m_terminated = false;
  m_rp      = NULL;
}

DebugThread::~DebugThread() {
}

unsigned int DebugThread::run() {
  setDeamon(true);
  m_oldState = (RemesState)-1;
  m_oldMainIteration    = -1;
  m_oldSearchEIteration = -1;
  m_oldExtremaCount     = -1;
  m_r.setHandler(this);
  try {
    setBoolProperty(THREAD_RUNNING   , m_running, true);
    m_r.solve(m_M, m_K);
  } catch(Exception e) {
    m_errorMsg = e.what();
    notifyPropertyChanged(THREAD_ERROR, _T(""), m_errorMsg.cstr());
  } catch(bool) {
    // ignore. thrown after resume, when killed
  }
  m_rp = &m_r;
  setBoolProperty(THREAD_TERMINATED, m_terminated, true );
  setBoolProperty(THREAD_RUNNING   , m_running   , false);
  return 0;
}

typedef enum {
  BREAKSTEP
 ,BREAKSUBSTEP
} BreakPointType;

void DebugThread::throwInvalidStateException(RemesState state) const {
  throwMethodInvalidArgumentException(s_className, _T("handleData"), _T("State=%d"), state);
}

void DebugThread::handleData(const Remes &r) {
  if(m_killed) throw true;

  m_rp = &r;
  const RemesState newState = r.getState();
  if(m_breakPoints.isEmpty()) {
    if(newState != m_oldState) {
      notifyPropertyChanged(REMES_STATE, &m_oldState, &newState);
    } else 
      switch(newState) {
      case REMES_INITIALIZE        :
        break;
      case REMES_SEARCH_E          :
        { const int newValue = r.getSearchEIteration();
          notifyPropertyChanged(REMES_SEARCHE_ITERATION, &m_oldSearchEIteration, &newValue);
          m_oldSearchEIteration = newValue;
        }
        break;

      case REMES_SEARCH_EXTREMA    :
        { const int newValue = r.getExtremaCount();
          notifyPropertyChanged(REMES_EXTREMA_COUNT, &m_oldExtremaCount, &newValue);
          m_oldExtremaCount = newValue;
        }
        break;

      case REMES_FINALIZE_ITERATION:
        { const int newValue = r.getMainIteration();
          notifyPropertyChanged(REMES_MAIN_ITERATION, &m_oldMainIteration, &newValue);
          m_oldMainIteration = newValue;
        }
        break;

      case REMES_NOCONVERGENCE     :
        break;

      default:
        throwInvalidStateException(newState);
      }
  } else {
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
      throwInvalidStateException(newState);
    }
  }
  m_oldState = newState;
  m_rp       = &m_r;
}

const Remes &DebugThread::getRemes() const {
  if(m_rp == NULL) {
    throwException(_T("DebugThread::getRemes()::Cannot access remes data in this state"));
  }
  return *m_rp;
}

void DebugThread::setBoolProperty(DebugThreadProperty id, bool &v, bool newValue) {
  if(newValue != v) {
    const bool oldValue = v;
    v = newValue;
    notifyPropertyChanged(id, &oldValue, &v);
  }
}

void DebugThread::stop() {
  setBoolProperty(THREAD_RUNNING, m_running, false);
  suspend();
  if(m_killed) throw true;
  setBoolProperty(THREAD_RUNNING, m_running, true);
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
