#pragma once

#include "WinTools.h"

class AbstractMouseHandler {
public:
  virtual void OnLButtonDown(  UINT nFlags, CPoint point) {}
  virtual void OnLButtonDblClk(UINT nFlags, CPoint point) {}
  virtual void OnLButtonUp(    UINT nFlags, CPoint point) {}
  virtual void OnRButtonDown(  UINT nFlags, CPoint point) {}
  virtual void OnRButtonUp(    UINT nFlags, CPoint point) {}
  virtual void OnMouseMove(    UINT nFlags, CPoint point) {}
  virtual BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) { return TRUE; }
  virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) { return FALSE; };
};
