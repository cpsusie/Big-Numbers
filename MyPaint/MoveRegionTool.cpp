#include "stdafx.h"
#include "DrawTool.h"

MoveRegionTool::MoveRegionTool(PixRectContainer *container) : DrawTool(container) {
  m_copy = m_mask = m_old = NULL;
}

MoveRegionTool::~MoveRegionTool() {
  if(m_copy != NULL) {
    invertPolygon();
    releaseCopy();
  }
}

void MoveRegionTool::releaseCopy() {
  SAFEDELETE(m_copy);
  SAFEDELETE(m_mask);
  SAFEDELETE(m_old );
  m_polygon.clear();
}

void MoveRegionTool::invertPolygon(bool redraw) {
  getImage()->polygon(m_polygon,BLACK,true,m_copy!=NULL);
  if(redraw) {
    repaint();
  }
}

void MoveRegionTool::OnLButtonDown(UINT nFlags, const CPoint &point) {
  if(m_copy == NULL) {
    m_polygon.add(point);
    invertPolygon();
  } else if(m_polygon.contains(point) == 1) {
    m_lastPoint = point;
    invertPolygon(); // remove polygon
  } else {
    invertPolygon(); // remove polygon
    releaseCopy();
    m_polygon.clear();
    m_polygon.add(point);
    invertPolygon();
  }
}

void MoveRegionTool::OnMouseMove(UINT nFlags, const CPoint &point) {
  if(nFlags & MK_LBUTTON) {
    if(m_copy == NULL) {
      invertPolygon(false); // remove polygon
      m_polygon.add(point);
      invertPolygon(); // draw polygon
    } else {
      m_container->saveDocState();

      getImage()->rop(m_rect.TopLeft(),m_rect.Size(),SRCCOPY, m_old,ORIGIN);
      m_rect += point - m_lastPoint;
      m_polygon.move(point - m_lastPoint);
      m_lastPoint = point;
      m_old->rop(ORIGIN,m_rect.Size(),SRCCOPY, getImage(),m_rect.TopLeft());
//      getImage()->rop(m_rect.TopLeft(),m_rect.Size(),DSTINVERT,NULL,ORIGIN);
//      getImage()->mask(m_rect.TopLeft(),m_rect.Size(),MAKEROP4(SRCCOPY,DSTINVERT), m_copy,ORIGIN, m_mask);
      getImage()->mask(m_rect.TopLeft(),m_rect.Size(),SRCCOPY, m_copy,ORIGIN, m_mask);
      repaint();
    }
  }
}

void MoveRegionTool::OnLButtonUp(UINT nFlags, const CPoint &point) {
  if(m_copy == NULL) {
    invertPolygon();   // remove polygon
    m_rect = m_polygon.getBoundsRect();                                                             // define rect
    if(m_polygon.size() > 2 && m_rect.Width() > 0 && m_rect.Height() > 0) {                         // if valid rectangle
      m_copy = theApp.fetchPixRect(m_rect.Size());                                                          //   define copy
      m_mask = theApp.fetchPixRect(m_rect.Size());                                                          //   define mask
      m_old  = theApp.fetchPixRect(m_rect.Size());                                                          //   define old
      m_copy->rop(ORIGIN,m_rect.Size(),SRCCOPY, getImage(),m_rect.TopLeft());                       //   take a copy
      m_old->rop( ORIGIN,m_rect.Size(),SRCCOPY, getImage(),m_rect.TopLeft());                       //   take a copy
      createMask();

      invertPolygon(); // draw polygon
    } else {
      m_polygon.clear();
    }
  } else {
    invertPolygon(); // draw polygon
  }
}

void MoveRegionTool::createMask() {
  MyPolygon maskPolygon = m_polygon;
  maskPolygon.move(-m_rect.TopLeft());
  m_mask->fillRect(ORIGIN,m_rect.Size(),BLACK);
  m_mask->fillPolygon(maskPolygon,WHITE);
}

int MoveRegionTool::getCursorId() const {
  return m_copy == NULL ? IDC_CURSORHAIRCROSS : IDC_CURSORMOVE4WAYS;
}
