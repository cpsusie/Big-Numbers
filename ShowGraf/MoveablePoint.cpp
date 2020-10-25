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
      text = getSystem().getValueText(XAXIS_INDEX, p.x());
      break;
    case SHOWYCOORDINATE    :
      text = getSystem().getValueText(YAXIS_INDEX, p.y());
      break;
    case SHOWBOTHCOORDINATES:
    default                 :
      text = getSystem().getPointText(p);
      break;
    }
    vp.TextOut(getLocation(), text, g->getParam().getColor(), &m_bckSave);
    vp.setDC(oldDC);
  }
}
