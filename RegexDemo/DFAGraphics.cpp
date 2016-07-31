#include "stdafx.h"
#include <Timer.h>
#include <Date.h>
#include "DFAPainter.h"

#ifdef _DEBUG

class BlinkingStateThread : private Thread, public TimeoutHandler {
private:
  CWnd     *m_wnd;
  Timer     m_timer;
  bool      m_colorOn;
  Semaphore m_trigger;
  int       m_state;
public:
  BlinkingStateThread(CWnd *wnd) : m_wnd(wnd), m_timer(1), m_trigger(0) {
    setDeamon(true);
    start();
  }
  void handleTimeout(Timer &timer) {
    m_trigger.signal();
  };

  UINT run();
  void startBlinking(int state);
  void stopBlinking();
};

UINT BlinkingStateThread::run() {
  for(;;) {
    m_trigger.wait();
    m_colorOn = !m_colorOn;
    if(!m_wnd->IsWindowVisible()) {
      stopBlinking();
      continue;
    }
    if(!DFAPainter::markState(CClientDC(m_wnd), m_state, m_colorOn)) {
      stopBlinking();
    };
  }
}

void BlinkingStateThread::startBlinking(int state) {
  m_colorOn = false;
  m_state = state;
  m_timer.startTimer(500, *this, true);
}

void BlinkingStateThread::stopBlinking() {
  m_timer.stopTimer();
}

static BlinkingStateThread *blinkerThread = NULL;

void DFA::paint(CWnd *wnd, int currentState, int lastAcceptState) const {
  DFAPainter dp(*this, getClientRect(wnd).Size());
  dp.calculateAllPositions();
  dp.shiftCurrentToNew();
  dp.paintNew(CClientDC(wnd), currentState);

  if(blinkerThread == NULL) {
    blinkerThread = new BlinkingStateThread(wnd);
  }
  if(lastAcceptState >= 0) {
    blinkerThread->startBlinking(lastAcceptState);
  } else {
    blinkerThread->stopBlinking();
  }
}

void DFAPainter::stopBlinker() { // static
  if(blinkerThread) {
    blinkerThread->stopBlinking();
  }
}
#endif
