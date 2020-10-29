#include "pch.h"
#include <MFCUtil/ColorSpace.h>
#include <MFCUtil/Viewport2D.h>

Viewport2D::Viewport2D(bool retainAspectRatio) {
  m_tr                = new Rectangle2DTransformation(); TRACE_NEW(m_tr);
  m_ownTransformation = true;
  m_dc                = nullptr;
  m_retainAspectRatio = retainAspectRatio;
  m_currentPenWidth   = 0;
  setCurrentPen(PS_SOLID, 1, BLACK);
}

Viewport2D::Viewport2D(CDC &dc, const Rectangle2D &from, const Rectangle2D &to, bool retainAspectRatio) : m_dc(&dc) {
  m_tr                = new Rectangle2DTransformation(from, to); TRACE_NEW(m_tr);
  m_ownTransformation = true;
  m_currentPenWidth   = 0;
  m_retainAspectRatio = retainAspectRatio;
  setCurrentPen(PS_SOLID, 1, BLACK);
  if(retainAspectRatio) {
    m_tr->adjustAspectRatio();
  }
}

Viewport2D::Viewport2D(CDC &dc, Rectangle2DTransformation &tr, bool retainAspectRatio) : m_dc(&dc) {
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
    m_dc->SelectClipRgn(nullptr);
  }
  if(m_clipRgn.m_hObject != nullptr) {
    m_clipRgn.DeleteObject();
  }
}

void Viewport2D::destroyTransformation() {
  if(m_ownTransformation && m_tr) {
    SAFEDELETE(m_tr);
  }
  m_tr = nullptr;
}

void Viewport2D::destroyPen() const {
  if(hasDC()) {
    m_dc->SelectObject((CPen*)nullptr);
  }
  if(m_currentPen.m_hObject != nullptr) {
    m_currentPen.DeleteObject();
  }
}

void Viewport2D::setClipping(bool clip) const {
  destroyClipRgn();
  if(clip) {
    const CRect clipRect = (CRect)getToRectangle();
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
  throwException(_T("%s:m_dc is nullptr"), method);
}
#endif

void Viewport2D::MoveTo(const Point2D &p) const {
  CHECKHASDC();
  m_dc->MoveTo((CPoint)forwardTransform(p));
}

void Viewport2D::LineTo(const Point2D &p) const {
  CHECKHASDC();
  m_dc->LineTo((CPoint)forwardTransform(p));
}

void Viewport2D::SetPixel(const Point2D &p, COLORREF color) const {
  CHECKHASDC();
  m_dc->SetPixel((CPoint)forwardTransform(p),color);
}

COLORREF Viewport2D::GetPixel(const Point2D &p) const {
  CHECKHASDC();
  return m_dc->GetPixel((CPoint)forwardTransform(p));
}

void Viewport2D::paintCross(const Point2D &point, COLORREF color, int size) const {
  CHECKHASDC();
  const CPoint p       = (CPoint)forwardTransform(point);
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
  const CRect tmp = (CRect)forwardTransform(r);
  return m_dc->Rectangle(&tmp) ? true : false;
}

void Viewport2D::FillSolidRect(const Rectangle2D &r, COLORREF color) const {
  CHECKHASDC();
  const CRect rect = (CRect)forwardTransform(r);
  m_dc->FillSolidRect(rect,color);
}

void Viewport2D::TextOut(const Point2D &point, const String &text, COLORREF color, BackgroundSaver *bckSave /* = nullptr*/) const {
  CHECKHASDC();
  const int      oldMode  = m_dc->SetBkMode(TRANSPARENT);
  const COLORREF oldColor = m_dc->SetTextColor(color);
  const CPoint   p        = (CPoint)forwardTransform(point);
  if(bckSave != nullptr) {
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
  const CRect r     = (CRect)forwardTransform(rect);
  const CRect lastr = (CRect)forwardTransform(lastRect);
  m_dc->DrawDragRect(r,size,lastr,lastSize,brush,lastBrush);
}
