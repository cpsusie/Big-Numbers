#include "stdafx.h"
#include "MouseTool.h"

void MouseTool::beginDragRect(UINT nFlags, const CPoint &point) {
  m_dragging           = true;
  m_mouseDownPoint     = point;
  m_mouseDownTransform = getSystem().getTransformation();
  CRect cr             = getSystemClientRect();
  getWin()->ClientToScreen(&cr);
  ClipCursor(&cr);
  setWindowCursor(OCR_HAND);
}

void MouseTool::endDragRect(UINT nFlags, const CPoint &point) {
  m_dragging = false;
  setWindowCursor(OCR_NORMAL);
  ClipCursor(NULL);
}

void MouseTool::updateDragRect(UINT nFlags, const CPoint &point) {
  if(m_dragging) {
    if(nFlags & MK_LBUTTON) {
      Rectangle2D   fr         = m_mouseDownTransform.getFromRectangle();
      Point2D       startPoint = m_mouseDownTransform.backwardTransform((Point2DP)m_mouseDownPoint);
      Point2D       newPoint   = m_mouseDownTransform.backwardTransform((Point2DP)point);
      const Point2D dp         = newPoint - startPoint;
      fr -= dp;
      try {
        getSystem().getTransformation().setFromRectangle(fr);
        repaint();
      } catch(Exception e) {
        // ignore
      }
    }
  }
}

void MouseTool::beginMarkInterval(UINT nFlags, const CPoint &point) {
  m_mouseDownPoint = point;
  m_dragging       = true;
  CRect cr         = getSystemClientRect();
  m_dragRect       = CRect(m_mouseDownPoint.x, cr.bottom, m_mouseDownPoint.x, cr.top);
  getWin()->ClientToScreen(&cr);
  ClipCursor(&cr);
  CClientDC(&getSystem()).DrawDragRect(&m_dragRect, CSize(1,1), NULL, CSize(1,1));
}

void MouseTool::endMarkInterval(UINT nFlags, const CPoint &point) {
  CClientDC(&getSystem()).DrawDragRect(&m_dragRect, CSize(1,1), NULL, CSize(1,1));
  m_dragging = false;
  ClipCursor(NULL);
}

void MouseTool::updateMarkInterval(UINT nFlags, const CPoint &point) {
  if(m_dragging) {
    if(nFlags & MK_LBUTTON) {
      const CRect cr      = getSystemClientRect();
      const CRect newRect = CRect(m_mouseDownPoint.x, cr.bottom, point.x, cr.top);
      CClientDC(&getSystem()).DrawDragRect(&newRect, CSize(1,1), &m_dragRect, CSize(1,1));
      m_dragRect = newRect;
    } else {
      CClientDC(&getSystem()).DrawDragRect(&m_dragRect, CSize(1,1), NULL, CSize(1,1));
      m_dragging = false;
      ClipCursor(NULL);
    }
  }
}

// ------------------------------------- DragTool -----------------------------------------------

void DragTool::OnLButtonDown(UINT nFlags, const CPoint &point) {
  beginDragRect(nFlags, point);
}

void DragTool::OnLButtonUp(UINT nFlags, const CPoint &point) {
  endDragRect(nFlags, point);
}

void DragTool::OnMouseMove(UINT nFlags, const CPoint &point) {
  updateDragRect(nFlags, point);
}

// ------------------------------------- SearchIntervalTool -----------------------------------------------

void SearchIntervalTool::OnLButtonDown(UINT nFlags, const CPoint &point) {
  beginMarkInterval(nFlags, point);
}

void SearchIntervalTool::OnLButtonUp(UINT nFlags, const CPoint &point) {
  endMarkInterval(nFlags, point);
  if(m_buttonUpmessage) {
    postButtonUpMessage();
  }
}

void SearchIntervalTool::postButtonUpMessage() {
  theApp.getMainWindow()->PostMessage(m_buttonUpmessage, getDragRect().left,getDragRect().right);
}

void SearchIntervalTool::OnMouseMove(UINT nFlags, const CPoint &point) {
  updateMarkInterval(nFlags, point);
}
