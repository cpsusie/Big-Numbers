#include "stdafx.h"
#include "MoveablePoint.h"

MoveablePoint::MoveablePoint(Graph *graph, MoveablePointType type, const Point2D &point, BYTE showFlags)
  : CoordinateSystemObject(graph->getSystem())
  , m_graph(graph)
  , m_type(type)
  , m_range(point)
  , m_showFlags(showFlags)
  , m_location(point)
{
  initTextRect();
}

void MoveablePoint::paint(CDC &dc) {
  const Graph *g = getGraph();
  if(g->isVisible()) {
    const Viewport2D  &vp = getViewport();
    CDC               *oldDC = vp.setDC(&dc);
    const Point2D p(m_range.getMinX(), m_range.getMinY());
    String text;
    switch(m_showFlags) {
    case SHOWXCOORDINATE    :
      text = getSystem().getValueText(XAXIS_INDEX, p.x);
      break;
    case SHOWYCOORDINATE    :
      text = getSystem().getValueText(YAXIS_INDEX, p.y);
      break;
    case SHOWBOTHCOORDINATES:
    default                 :
      text = getSystem().getPointText(p);
      break;
    }
    vp.TextOut(getLocation(), text, g->getParam().getColor(), &m_textRect, &m_saveBM);
    vp.setDC(oldDC);
  }
}

void MoveablePoint::unpaint(CDC &dc) {
  if(hasTextRect()) {
    CDC tmpDC;
    tmpDC.CreateCompatibleDC(NULL);
    CBitmap *oldBM = tmpDC.SelectObject(&m_saveBM);
    dc.BitBlt(m_textRect.left, m_textRect.top, m_textRect.Width(), m_textRect.Height(), &tmpDC, 0,0, SRCCOPY);
    tmpDC.SelectObject(oldBM);
    tmpDC.DeleteDC();
    initTextRect();
  }
}
