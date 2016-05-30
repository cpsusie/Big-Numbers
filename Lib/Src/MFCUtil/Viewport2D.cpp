#include "stdafx.h"
#include <MFCUtil/Viewport2D.h>

Viewport2D::Viewport2D(bool retainAspectRatio) {
  m_tr                = new RectangleTransformation();
  m_ownTransformation = true;
  m_dc                = NULL;
  m_retainAspectRatio = retainAspectRatio;
  m_currentPenWidth   = 0;
  setCurrentPen(PS_SOLID, 1, RGB(0,0,0));
}

Viewport2D::Viewport2D(CDC &dc, const Rectangle2DR &from, const Rectangle2DR &to, bool retainAspectRatio) : m_dc(&dc) {
  m_tr                = new RectangleTransformation(from, to);
  m_ownTransformation = true;
  m_currentPenWidth   = 0;
  m_retainAspectRatio = retainAspectRatio;
  setCurrentPen(PS_SOLID, 1, RGB(0,0,0));
  if(retainAspectRatio) {
    m_tr->adjustAspectRatio();
  }
}

Viewport2D::Viewport2D(CDC &dc, RectangleTransformation &tr, bool retainAspectRatio) : m_dc(&dc) {
  m_tr                = &tr; 
  m_ownTransformation = false;
  m_currentPenWidth   = 0;
  setCurrentPen(PS_SOLID,1,RGB(0,0,0));
  if(retainAspectRatio) {
    m_tr->adjustAspectRatio();
  }
}

Viewport2D::~Viewport2D() {
  destroyClipRgn();
  destroyTransformation();
  destroyPen();
}

void Viewport2D::destroyClipRgn() {
  if(hasDC()) {
    m_dc->SelectClipRgn(NULL);
  }
  if(m_clipRgn.m_hObject != NULL) {
    m_clipRgn.DeleteObject();
  }
}

void Viewport2D::destroyTransformation() {
  if(m_ownTransformation && m_tr) {
    delete m_tr;
  }
  m_tr = NULL;
}

void Viewport2D::destroyPen() {
  if(hasDC()) {
    m_dc->SelectObject((CPen*)NULL);
  }
  if(m_currentPen.m_hObject != NULL) {
    m_currentPen.DeleteObject();
  }
}

void Viewport2D::setClipping(bool clip) {
  destroyClipRgn();
  if(clip) {
    const CRect clipRect = getToRectangle();
    m_clipRgn.CreateRectRgnIndirect(&clipRect);
    if(hasDC()) {
      m_dc->SelectClipRgn(&m_clipRgn);
    }
  }
}

CPen &Viewport2D::setCurrentPen(int penStyle, int width, COLORREF color) {
  if((penStyle != m_currentPenStyle) || (width != m_currentPenWidth) || (color != m_currentPenColor)) {
    destroyPen();
    m_currentPen.CreatePen(penStyle,width,color);
    m_currentPenStyle = penStyle;
    m_currentPenWidth = width;
    m_currentPenColor = color;
  }
  return m_currentPen;
}

void Viewport2D::setToRectangle(const Rectangle2DR &rect) {
  m_tr->setToRectangle(rect);
  if(isRetainingAspectRatio()) {
    m_tr->adjustAspectRatio();
  }
}

void Viewport2D::setRetainAspectRatio(bool retainAspectRatio) {
  m_retainAspectRatio = retainAspectRatio;
  if(m_retainAspectRatio) {
    m_tr->adjustAspectRatio();
  }
}

void Viewport2D::MoveTo(const Point2DP &p) {
  m_dc->MoveTo(forwardTransform(p));
}

void Viewport2D::MoveTo(double x, double y) {
  m_dc->MoveTo(forwardTransform(x,y));
}

void Viewport2D::LineTo(const Point2DP &p) {
  m_dc->LineTo(forwardTransform(p));
}

void Viewport2D::LineTo(double x, double y) {
  m_dc->LineTo(forwardTransform(x,y));
}

void Viewport2D::SetPixel(const Point2DP &p, COLORREF color) {
  m_dc->SetPixel(forwardTransform(p),color);
}

void Viewport2D::SetPixel(double x, double y, COLORREF color) {
  m_dc->SetPixel(forwardTransform(x,y),color);
}

COLORREF Viewport2D::GetPixel(const Point2DP &p) {
  return m_dc->GetPixel(forwardTransform(p));
}

COLORREF Viewport2D::GetPixel(double x, double y) {
  return m_dc->GetPixel(forwardTransform(x,y));
}

void Viewport2D::paintLine(int x1, int y1, int x2, int y2, int penStyle, COLORREF color) {
  CPen *origPen = m_dc->SelectObject(&setCurrentPen(penStyle,1,color));
  m_dc->MoveTo(x1,y1);
  m_dc->LineTo(x2,y2);
  m_dc->SelectObject(origPen);
}

void Viewport2D::paintCross(const Point2DP &point, COLORREF color, int size) {
  CPoint p = forwardTransform(point);
  if(size%1) {
    const int d = size/2+1;
    paintLine(p.x-d,p.y-d,p.x+d,p.y+d,PS_SOLID,color);
    paintLine(p.x+d,p.y-d,p.x-d,p.y+d,PS_SOLID,color);
  } else {
    const int d = size/2, d1 = d+1;
    paintLine(p.x-d,p.y-d,p.x+d1,p.y+d1,PS_SOLID,color);
    paintLine(p.x+d,p.y-d,p.x-d1,p.y+d1,PS_SOLID,color);
  }
}

bool Viewport2D::Rectangle(const Rectangle2DR &r) {
  CRect tmp = forwardTransform(r);
  return m_dc->Rectangle(&tmp) ? true : false;
}

bool Viewport2D::Rectangle(double x1, double y1, double x2, double y2) {
  int ix1 = (int)m_tr->getXTransformation().forwardTransform(x1);
  int iy1 = (int)m_tr->getYTransformation().forwardTransform(y1);
  int ix2 = (int)m_tr->getXTransformation().forwardTransform(x2);
  int iy2 = (int)m_tr->getYTransformation().forwardTransform(y2);
  return m_dc->Rectangle(ix1,iy1,ix2,iy2) ? true : false;
}

void Viewport2D::FillSolidRect(const Rectangle2DR &r, COLORREF color) {
  CRect rect = forwardTransform(r);
  m_dc->FillSolidRect(rect,color);
}

void Viewport2D::clear(COLORREF color) {
  FillSolidRect(m_tr->getFromRectangle(),color);
}

CBitmap *Viewport2D::SelectObject(CBitmap *bitmap) {
  return m_dc->SelectObject(bitmap);
}

CPen *Viewport2D::SelectObject(CPen *pen) {
  return m_dc->SelectObject(pen);
}

CGdiObject *Viewport2D::SelectObject(CGdiObject *object) {
  return m_dc->SelectObject(object);
}

CGdiObject *Viewport2D::SelectStockObject(int index) {
  return m_dc->SelectStockObject(index);
}

void Viewport2D::paintDragRect(const Rectangle2D &rect, SIZE size, const Rectangle2D &lastRect, SIZE lastSize, CBrush *brush, CBrush *lastBrush) {
  CRect r     = forwardTransform(rect);
  CRect lastr = forwardTransform(lastRect);
  m_dc->DrawDragRect(r,size,lastr,lastSize,brush,lastBrush);
}

CRect makePositiveRect(const CRect &r) {
  CRect result = r;
  if (result.Width() < 0) {
    swap(result.left, result.right);
  }
  if (result.Height() < 0) {
    swap(result.top, result.bottom);
  }
  return result;
}

