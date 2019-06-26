#include "stdafx.h"
#include "MoveablePoint.h"

void MoveablePoint::paint(CCoordinateSystem &cs) {
  Viewport2D  &vp = cs.getViewport();
  const Graph *g  = getGraph();
  if(g->isVisible()) {
    vp.setClipping(true);
    const Point2D p(m_range.getMinX(), m_range.getMinY());
    String text;
    switch(m_showFlags) {
    case SHOWXCOORDINATE    :
      text = cs.getValueText(XAXIS_INDEX, p.x);
      break;
    case SHOWYCOORDINATE    :
      text = cs.getValueText(YAXIS_INDEX, p.y);
      break;
    case SHOWBOTHCOORDINATES:
    default                 :
      text = cs.getPointText(p);
      break;
    }
    vp.TextOut(m_location, text, g->getParam().getColor());
    vp.setClipping(false);
  }
}
