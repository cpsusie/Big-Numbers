#include "stdafx.h"
#include "DrawTool.h"

void PolygonTool::invertDragLine() {
  getImage()->line(m_polygon.last(),m_p0,0,true);
  repaint();
}

void PolygonTool::OnLButtonDown(UINT nFlags, const CPoint &point) {
  if(m_polygon.size() == 0) {
    m_container->saveDocState();
    m_polygon.add(point);
    m_p0 = point;
    invertDragLine(); // draw dragline
  } else {
    invertDragLine(); // remove dragline
    m_p0 = point;
    getImage()->line(m_polygon.last(),m_p0,m_container->getColor());
    m_polygon.add(point);
    invertDragLine(); // draw dragline
  }
  repaint();
}

void PolygonTool::OnLButtonDblClk(UINT nFlags, const CPoint &point) {
  if(m_polygon.size() == 1) {
    invertDragLine(); // remove dragline
  } else if(m_polygon.size() > 1) {
    invertDragLine(); // remove dragline
    getImage()->line(m_polygon.last(),m_polygon[0],m_container->getColor());
    getImage()->fillPolygon(m_polygon,m_container->getColor());
  }
  repaint();
  m_polygon.clear();
}

void PolygonTool::OnMouseMove(UINT nFlags, const CPoint &point) {
  if(m_polygon.size() > 0) {
    if(nFlags & MK_LBUTTON) {
      invertDragLine(); // remove dragline
      m_p0 = point;
      invertDragLine(); // draw dragline
      repaint();
    } else {
      m_p0 = point;
    }
  }
}

void PolygonTool::OnLButtonUp(UINT nFlags, const CPoint &point) {
  if(m_polygon.size() == 1) {
    invertDragLine(); // remove dragline
    if(point == m_polygon.first()) {
      m_polygon.clear();
    } else {
      m_p0 = point;
      getImage()->line(m_polygon.last(),m_p0,m_container->getColor());
      m_polygon.add(point);
      invertDragLine(); // draw dragline
    }
  } else if(m_polygon.size() > 1) {
    invertDragLine(); // remove dragline
  }
  repaint();
}
