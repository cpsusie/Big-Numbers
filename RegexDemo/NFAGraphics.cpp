#include "stdafx.h"
#include "NFAPainter.h"

#ifdef _DEBUG

void NFAPaint(CWnd *wnd) {
  NFAPainter np(getClientRect(wnd).Size());
  np.calculateAllPositions();
  np.shiftCurrentToNew();
  np.paintNew(CClientDC(wnd));
}

void NFAAnimateBuildStep(CWnd *wnd) {
  NFAPainter np(getClientRect(wnd).Size());
  np.calculateAllPositions();
  np.shiftCurrentToNew();
  np.animateOldToNew(CClientDC(wnd));
}

#endif
