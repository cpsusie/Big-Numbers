#include "stdafx.h"
#include <Date.h>
#include "DFAPainter.h"

#ifdef _DEBUG

void DFAStateBlinker::startBlinking(CWnd *wnd, int state) {
  m_wnd     = wnd;
  m_colorOn = false;
  m_state   = state;
  m_timer.startTimer(500, *this, true);
}

void DFAStateBlinker::stopBlinking() {
  m_timer.stopTimer();
}

void DFAStateBlinker::handleTimeout(Timer &timer) {
  m_colorOn = !m_colorOn;
  if(!m_wnd->IsWindowVisible()) {
    stopBlinking();
    return;
  }
  if(!DFAPainter::markState(CClientDC(m_wnd), m_state, m_colorOn)) {
    stopBlinking();
  }
}

DFAStateBlinker DFAPainter::s_blinker;

void DFA::paint(CWnd *wnd, int currentState, int lastAcceptState) const {
  DFAPainter dp(*this, getClientRect(wnd).Size());
  dp.calculateAllPositions();
  dp.shiftCurrentToNew();
  dp.paintNew(CClientDC(wnd), currentState);

  if(lastAcceptState >= 0) {
    DFAPainter::startBlinking(wnd,lastAcceptState);
  } else {
    DFAPainter::stopBlinking();
  }
}

#endif
