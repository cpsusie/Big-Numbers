#pragma once

#include "Transformation.h"

class Viewport2D {
private:
  CDC                     *m_dc;
  RectangleTransformation *m_tr;
  bool                     m_ownTransformation;
  bool                     m_retainAspectRatio;
  CRgn                     m_clipRgn;

  CPen                     m_currentPen;
  int                      m_currentPenStyle;
  int                      m_currentPenWidth;
  COLORREF                 m_currentPenColor;

  CPen &setCurrentPen(int penStyle, int width, COLORREF color);
  void paintLine(int x1, int y1, int x2, int y2, int penStyle, COLORREF color);
  void destroyClipRgn();
  void destroyTransformation();
  void destroyPen();
  bool hasDC() const {
    return m_dc && m_dc->m_hDC;
  }
public:
  Viewport2D(bool retainAspectRatio = false);
  Viewport2D(CDC &dc, const Rectangle2D &from, const Rectangle2D &to, bool retainAspectRatio = false);
  Viewport2D(CDC &dc, RectangleTransformation &tr,  bool retainAspectRatio = false);
  ~Viewport2D();

  CDC *getDC() {
    return m_dc;
  }

  void setDC(CDC *dc) {
    m_dc = dc;
  }

  RectangleTransformation &getTransformation() {
    return *m_tr;
  }

  const RectangleTransformation &getTransformation() const {
    return *m_tr;
  }

  Rectangle2D getFromRectangle() const {
    return m_tr->getFromRectangle();
  }

  Rectangle2D getToRectangle() const {
    return m_tr->getToRectangle();
  }

  void setFromRectangle(const Rectangle2D &rect) {
    m_tr->setFromRectangle(rect);
  }
  
  void setToRectangle(const Rectangle2D &rect);
  
  void setScale(IntervalScale newScale, int flags) {
    m_tr->setScale(newScale, flags);
  }

  void setRetainAspectRatio(bool retainAspectRatio);

  bool isRetainingAspectRatio() const {
    return m_retainAspectRatio;
  }

  const IntervalTransformation &getXTransformation() const {
    return m_tr->getXTransformation();
  }
  
  const IntervalTransformation &getYTransformation() const {
    return m_tr->getYTransformation();
  }
  
  Point2D forwardTransform(const Point2D &p) const {
    return m_tr->forwardTransform(p);
  }

  Point2D backwardTransform(const Point2D &p) const {
    return m_tr->backwardTransform(p);
  }

  Rectangle2D forwardTransform(const Rectangle2D &r) const {
    return m_tr->forwardTransform(r);
  }

  Rectangle2D backwardTransform(const Rectangle2D &r) const {
    return m_tr->backwardTransform(r);
  }

  void        setClipping(bool clip);
  void        MoveTo(      const Point2D &p);
  void        MoveTo(double x, double y);
  void        LineTo(      const Point2D &p);
  void        LineTo(double x, double y);
  void        SetPixel(    const Point2D &p, COLORREF color);
  void        SetPixel(double x, double y, COLORREF color);
  void        paintCross(  const Point2D &p, COLORREF color, int size=4);
  COLORREF    GetPixel(const Point2D &p);
  COLORREF    GetPixel(double x, double y);
  bool        Rectangle(const Rectangle2D &r);
  bool        Rectangle(double x1, double y1, double x2, double y2);
  void        FillSolidRect(const Rectangle2D &r, COLORREF color);
  void        clear(COLORREF color);
  CGdiObject *SelectObject(CGdiObject *object);
  CBitmap    *SelectObject(CBitmap    *bitmap);
  CPen       *SelectObject(CPen       *pen);

  CGdiObject *SelectStockObject(int index);
  void paintDragRect(const Rectangle2D &rect, SIZE size, const Rectangle2D &lastRect, SIZE lastSize, CBrush *brush = NULL, CBrush *lastBrush = NULL);
};
