#include "stdafx.h"
#include <Date.h>
#include "DFAPainter.h"

#ifdef _DEBUG

void DFA::paint(CWnd *wnd, CDC &dc, int currentState, int lastAcceptState) const {
  DFAPainter dp(*this, getClientRect(wnd).Size());
  dp.calculateAllPositions();
  dp.shiftCurrentToNew();
  dp.paintNew(dc, currentState);

  if(lastAcceptState >= 0) {
    DFAPainter::startBlinking(wnd, lastAcceptState);
  } else {
    DFAPainter::stopBlinking();
  }
}

void DFA::unmarkAll() { // static
  DFAPainter::stopBlinking();
}

#endif
