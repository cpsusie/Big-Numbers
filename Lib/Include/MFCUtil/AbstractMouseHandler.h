#pragma once

#include "WinTools.h"

class AbstractMouseHandler {
public:
  virtual bool OnLButtonDown(  UINT nFlags, CPoint point) { return false; }
  virtual bool OnLButtonDblClk(UINT nFlags, CPoint point) { return false; }
  virtual bool OnLButtonUp(    UINT nFlags, CPoint point) { return false; }
  virtual bool OnRButtonDown(  UINT nFlags, CPoint point) { return false; }
  virtual bool OnRButtonUp(    UINT nFlags, CPoint point) { return false; }
  virtual bool OnMouseMove(    UINT nFlags, CPoint point) { return false; }
  virtual bool OnMouseWheel(   UINT nFlags, short zDelta, CPoint pt)  { return false; }
  virtual bool OnKeyDown(      UINT nChar, UINT nRepCnt, UINT nFlags) { return false; };
  virtual bool OnKeyUp(        UINT nChar, UINT nRepCnt, UINT nFlags) { return false; };
};
