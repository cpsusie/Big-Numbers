#include "stdafx.h"
#include "MouseTool.h"

String toString(MouseToolType type) {
  switch(type) {
#define CASEESTR(t) case t: return _T(#t)
  CASEESTR(IDLETOOL            );
  CASEESTR(DRAGTOOL            );
  CASEESTR(MOVEPOINTTOOL       );
  CASEESTR(FINDZEROTOOL        );
  CASEESTR(FINDMAXTOOL         );
  CASEESTR(FINDMINTOOL         );
  CASEESTR(FINDINTERSECTIONTOOL);
  default:
    return format(_T("Unknown MouseToolType:%d"), type);
  }
#undef CASESTR
}

void MouseTool::clipCursor() {
  CRect cr = getSystemClientRect();
  getWin()->ClientToScreen(&cr);
  ClipCursor(&cr);
}

void MouseTool::unClipCursor() {
  ClipCursor(NULL);
}

void MouseTool::beginDragRect(UINT nFlags, const CPoint &point) {
  m_dragging           = true;
  m_mouseDownPoint     = point;
  m_mouseDownTransform = getSystem().getTransformation();
  clipCursor();
  setWindowCursor(OCR_HAND);
}

void MouseTool::endDragRect() {
  m_dragging = false;
  setWindowCursor(OCR_NORMAL);
  unClipCursor();
}

void MouseTool::beginMarkInterval(UINT nFlags, const CPoint &point) {
  m_mouseDownPoint = point;
  m_dragging       = true;
  CRect cr         = getSystemClientRect();
  m_dragRect       = CRect(m_mouseDownPoint.x, cr.bottom, m_mouseDownPoint.x, cr.top);
  CClientDC(&getSystem()).DrawDragRect(&m_dragRect, CSize(1,1), NULL, CSize(1,1));
  clipCursor();
}

void MouseTool::endMarkInterval() {
  CClientDC(&getSystem()).DrawDragRect(&m_dragRect, CSize(1,1), NULL, CSize(1,1));
  m_dragging = false;
  unClipCursor();
}

void MouseTool::updateMarkInterval(UINT nFlags, const CPoint &point) {
  if(isDragging()) {
    if(nFlags & MK_LBUTTON) {
      const CRect cr      = getSystemClientRect();
      const CRect newRect = CRect(m_mouseDownPoint.x, cr.bottom, point.x, cr.top);
      CClientDC(&getSystem()).DrawDragRect(&newRect, CSize(1,1), &m_dragRect, CSize(1,1));
      m_dragRect = newRect;
    } else {
      endMarkInterval();
    }
  }
}

// ------------------------------------- IdleTool -----------------------------------------------

void IdleTool::OnLButtonDown(UINT nFlags, const CPoint &point) {
  GraphArray &ga = getDoc()->getGraphArray();
  ga.OnLButtonDown(nFlags, point);
  const SelectedData selection = ga.getCurrentSelection();
  switch(selection.getType()) {
  case NOSELECTION  :
  case GRAPHSELECTED:
    beginDragRect(nFlags, point);
    postMsg(ID_MSG_PUSHDRAGTOOL);
    break;
  case POINTSELECTED:
    beginDragRect(nFlags, point);
    postMsg(ID_MSG_PUSHMOVEPOINTTOOL);
    break;
  default           :
    break;
  }
}

// ------------------------------------- DragTool -----------------------------------------------

void DragTool::takeoverDrag(MouseTool &mdTool) {
  assert(mdTool.isDragging());
  assert(!isDragging());
  const CPoint p = mdTool.getMouseDownPoint();
  mdTool.endDragRect();
  beginDragRect(0, p);
}

void DragTool::updateDragRect(UINT nFlags, const CPoint &point) {
  if(isDragging()) {
    if(nFlags & MK_LBUTTON) {
      Rectangle2D   fr = m_mouseDownTransform.getFromRectangle();
      Point2D       startPoint = m_mouseDownTransform.backwardTransform((Point2DP)m_mouseDownPoint);
      Point2D       newPoint   = m_mouseDownTransform.backwardTransform((Point2DP)point);
      const Point2D dp = newPoint - startPoint;
      fr -= dp;
      try {
        getSystem().getTransformation().setFromRectangle(fr);
        repaint();
      }
      catch (Exception e) {
        // ignore
      }
    } else {
      endDragRect();
      postMsg(ID_MSG_POPMOUSETOOL);
    }
  }
}

void DragTool::OnMouseMove(UINT nFlags, const CPoint &point) {
  updateDragRect(nFlags, point);
}

void DragTool::OnLButtonUp(UINT nFlags, const CPoint &point) {
  endDragRect();
  postMsg(ID_MSG_POPMOUSETOOL);
}

// ------------------------------------- MovePointTool -----------------------------------------------

void MovePointTool::takeoverDrag(MouseTool &mdTool) {
  assert(mdTool.isDragging());
  assert(!isDragging());
  const CPoint p = mdTool.getMouseDownPoint();
  mdTool.endDragRect();
  beginMovePoint(p);
}

void MovePointTool::beginMovePoint(const CPoint &point) {
  m_dragging           = true;
  m_ga                 = &getDoc()->getGraphArray();
  m_mp                 = m_ga->getSelectedPoint();
  if(m_mp == NULL) {
    postMsg(ID_MSG_POPMOUSETOOL);
  } else {
    m_offset = point - m_mp->getTrLocation();
    clipCursor();
    setWindowCursor(OCR_HAND);
  }
}

void MovePointTool::endMovePoint() {
  m_dragging = false;
  setWindowCursor(OCR_NORMAL);
  unClipCursor();
}

void MovePointTool::movePoint(UINT nFlags, const CPoint &point) {
  if(isDragging()) {
    if(nFlags & MK_LBUTTON) {
      try {
        CClientDC dc(&getSystem());
        m_ga->unpaintPointArray(dc);
        m_mp->setTrLocation(point - m_offset);
        m_ga->paintPointArray(dc);
      } catch (Exception e) {
        // ignore
      }
    } else {
      endMovePoint();
      postMsg(ID_MSG_POPMOUSETOOL);
    }
  }
}

void MovePointTool::OnMouseMove(UINT nFlags, const CPoint &point) {
  movePoint(nFlags, point);
}

void MovePointTool::OnLButtonUp(UINT nFlags, const CPoint &point) {
  endMovePoint();
  postMsg(ID_MSG_POPMOUSETOOL);
}

// ------------------------------------- SearchIntervalTool -----------------------------------------------

void SearchIntervalTool::OnLButtonDown(UINT nFlags, const CPoint &point) {
  beginMarkInterval(nFlags, point);
}

void SearchIntervalTool::OnLButtonUp(UINT nFlags, const CPoint &point) {
  endMarkInterval();
  if(m_buttonUpmessage) {
    postButtonUpMessage();
  }
  postMsg(ID_MSG_POPMOUSETOOL);
}

void SearchIntervalTool::postButtonUpMessage() {
  postMsg(m_buttonUpmessage, getDragRect().left,getDragRect().right);
}

void SearchIntervalTool::OnMouseMove(UINT nFlags, const CPoint &point) {
  updateMarkInterval(nFlags, point);
}
