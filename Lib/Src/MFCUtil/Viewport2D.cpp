#include "pch.h"
#include <MFCUtil/ColorSpace.h>
#include <MFCUtil/Viewport2D.h>

Viewport2D::Viewport2D(bool retainAspectRatio) {
  m_tr                = new RectangleTransformation(); TRACE_NEW(m_tr);
  m_ownTransformation = true;
  m_dc                = NULL;
  m_retainAspectRatio = retainAspectRatio;
  m_currentPenWidth   = 0;
  setCurrentPen(PS_SOLID, 1, BLACK);
}

Viewport2D::Viewport2D(CDC &dc, const Rectangle2D &from, const Rectangle2D &to, bool retainAspectRatio) : m_dc(&dc) {
  m_tr                = new RectangleTransformation(from, to); TRACE_NEW(m_tr);
  m_ownTransformation = true;
  m_currentPenWidth   = 0;
  m_retainAspectRatio = retainAspectRatio;
  setCurrentPen(PS_SOLID, 1, BLACK);
  if(retainAspectRatio) {
    m_tr->adjustAspectRatio();
  }
}

Viewport2D::Viewport2D(CDC &dc, RectangleTransformation &tr, bool retainAspectRatio) : m_dc(&dc) {
  m_tr                = &tr;
  m_ownTransformation = false;
  m_currentPenWidth   = 0;
  setCurrentPen(PS_SOLID, 1, BLACK);
  if(retainAspectRatio) {
    m_tr->adjustAspectRatio();
  }
}

Viewport2D::~Viewport2D() {
  destroyClipRgn();
  destroyTransformation();
  destroyPen();
}

void Viewport2D::destroyClipRgn() const {
  if(hasDC()) {
    m_dc->SelectClipRgn(NULL);
  }
  if(m_clipRgn.m_hObject != NULL) {
    m_clipRgn.DeleteObject();
  }
}

void Viewport2D::destroyTransformation() {
  if(m_ownTransformation && m_tr) {
    SAFEDELETE(m_tr);
  }
  m_tr = NULL;
}

void Viewport2D::destroyPen() const {
  if(hasDC()) {
    m_dc->SelectObject((CPen*)NULL);
  }
  if(m_currentPen.m_hObject != NULL) {
    m_currentPen.DeleteObject();
  }
}

void Viewport2D::setClipping(bool clip) const {
  destroyClipRgn();
  if(clip) {
    const CRect clipRect = getToRectangle();
    m_clipRgn.CreateRectRgnIndirect(&clipRect);
    if(hasDC()) {
      m_dc->SelectClipRgn(&m_clipRgn);
    }
  }
}

CPen &Viewport2D::setCurrentPen(int penStyle, int width, COLORREF color) const {
  if((penStyle != m_currentPenStyle) || (width != m_currentPenWidth) || (color != m_currentPenColor)) {
    destroyPen();
    m_currentPen.CreatePen(penStyle,width,color);
    m_currentPenStyle = penStyle;
    m_currentPenWidth = width;
    m_currentPenColor = color;
  }
  return m_currentPen;
}

void Viewport2D::setToRectangle(const Rectangle2D &rect) {
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

#if defined(_DEBUG)
void Viewport2D::noDCError(const TCHAR *method) const {
  throwException(_T("%s:m_dc is NULL"), method);
}
#endif

void Viewport2D::MoveTo(const Point2D &p) const {
  CHECKHASDC();
  m_dc->MoveTo(forwardTransform(p));
}

void Viewport2D::MoveTo(double x, double y) const {
  CHECKHASDC();
  m_dc->MoveTo(forwardTransform(x,y));
}

void Viewport2D::LineTo(const Point2D &p) const {
  CHECKHASDC();
  m_dc->LineTo(forwardTransform(p));
}

void Viewport2D::LineTo(double x, double y) const{
  CHECKHASDC();
  m_dc->LineTo(forwardTransform(x,y));
}

void Viewport2D::SetPixel(const Point2D &p, COLORREF color) const {
  CHECKHASDC();
  m_dc->SetPixel(forwardTransform(p),color);
}

void Viewport2D::SetPixel(double x, double y, COLORREF color) const {
  CHECKHASDC();
  m_dc->SetPixel(forwardTransform(x,y),color);
}

COLORREF Viewport2D::GetPixel(const Point2D &p) const {
  CHECKHASDC();
  return m_dc->GetPixel(forwardTransform(p));
}

COLORREF Viewport2D::GetPixel(double x, double y) const {
  CHECKHASDC();
  return m_dc->GetPixel(forwardTransform(x,y));
}

void Viewport2D::paintCross(const Point2D &point, COLORREF color, int size) const {
  CHECKHASDC();
  const CPoint p       = forwardTransform(point);
  CPen        *origPen = m_dc->SelectObject(&setCurrentPen(PS_SOLID, 1, color));
  if(size%2) {
    const int d = size/2+1;
    paintLine(p.x-d,p.y-d,p.x+d,p.y+d);
    paintLine(p.x+d,p.y-d,p.x-d,p.y+d);
  } else {
    const int d = size/2, d1 = d+1;
    paintLine(p.x-d,p.y-d,p.x+d1,p.y+d1);
    paintLine(p.x+d,p.y-d,p.x-d1,p.y+d1);
  }
  m_dc->SelectObject(origPen);
}

bool Viewport2D::Rectangle(const Rectangle2D &r) const {
  CHECKHASDC();
  const CRect tmp = forwardTransform(r);
  return m_dc->Rectangle(&tmp) ? true : false;
}

bool Viewport2D::Rectangle(double x1, double y1, double x2, double y2) const {
  CHECKHASDC();
  const int ix1 = (int)m_tr->getXTransformation().forwardTransform(x1);
  const int iy1 = (int)m_tr->getYTransformation().forwardTransform(y1);
  const int ix2 = (int)m_tr->getXTransformation().forwardTransform(x2);
  const int iy2 = (int)m_tr->getYTransformation().forwardTransform(y2);
  return m_dc->Rectangle(ix1,iy1,ix2,iy2) ? true : false;
}

void Viewport2D::FillSolidRect(const Rectangle2D &r, COLORREF color) const {
  CHECKHASDC();
  const CRect rect = forwardTransform(r);
  m_dc->FillSolidRect(rect,color);
}

void Viewport2D::TextOut(const Point2D &point, const String &text, COLORREF color, BackgroundSaver *bckSave /* = NULL*/) const {
  CHECKHASDC();
  const int      oldMode  = m_dc->SetBkMode(TRANSPARENT);
  const COLORREF oldColor = m_dc->SetTextColor(color);
  const CPoint   p        = forwardTransform(point);
  if(bckSave != NULL) {
    bckSave->saveBackground(*m_dc, CRect(p, getTextExtent(*m_dc, text)));
  }
  textOut(*m_dc, p, text);
  m_dc->SetTextColor(oldColor);
  m_dc->SetBkMode(   oldMode );
}

void Viewport2D::clear(COLORREF color) const {
  FillSolidRect(m_tr->getFromRectangle(),color);
}

CBitmap *Viewport2D::SelectObject(CBitmap *bitmap) const {
  CHECKHASDC();
  return m_dc->SelectObject(bitmap);
}

CPen *Viewport2D::SelectObject(CPen *pen) const {
  CHECKHASDC();
  return m_dc->SelectObject(pen);
}

CFont *Viewport2D::SelectObject(CFont *font) const {
  CHECKHASDC();
  return m_dc->SelectObject(font);
}

CGdiObject *Viewport2D::SelectObject(CGdiObject *object) const {
  CHECKHASDC();
  return m_dc->SelectObject(object);
}

CGdiObject *Viewport2D::SelectStockObject(int index) const {
  CHECKHASDC();
  return m_dc->SelectStockObject(index);
}

void Viewport2D::paintDragRect(const Rectangle2D &rect, SIZE size, const Rectangle2D &lastRect, SIZE lastSize, CBrush *brush, CBrush *lastBrush) const {
  CHECKHASDC();
  const CRect r     = forwardTransform(rect);
  const CRect lastr = forwardTransform(lastRect);
  m_dc->DrawDragRect(r,size,lastr,lastSize,brush,lastBrush);
}
