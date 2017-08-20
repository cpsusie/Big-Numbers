#include "stdafx.h"
#include "NFAPainter.h"

#ifdef _DEBUG

void NFAPaint(CWnd *wnd, CDC &dc) {
  NFAPainter np(getClientRect(wnd).Size());
  np.calculateAllPositions();
  np.shiftCurrentToNew();
  np.paintNew(dc);
}

void NFAAnimateBuildStep(CWnd *wnd) {
  NFAPainter np(getClientRect(wnd).Size());
  np.calculateAllPositions();
  np.shiftCurrentToNew();
  np.animateOldToNew(CClientDC(wnd));
}

#endif
