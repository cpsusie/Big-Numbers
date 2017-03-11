#pragma once

#include <Math/Transformation.h>
#include "WinTools.h"
#include "Point2DP.h"

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
  inline bool hasDC() const {
    return m_dc && m_dc->m_hDC;
  }
public:
  Viewport2D(bool retainAspectRatio = false);
  Viewport2D(CDC &dc, const Rectangle2DR &from, const Rectangle2DR &to, bool retainAspectRatio = false);
  Viewport2D(CDC &dc, RectangleTransformation &tr,  bool retainAspectRatio = false);
  ~Viewport2D();

  inline CDC *getDC() {
    return m_dc;
  }

  inline void setDC(CDC *dc) {
    m_dc = dc;
  }

  inline RectangleTransformation &getTransformation() {
    return *m_tr;
  }

  inline const RectangleTransformation &getTransformation() const {
    return *m_tr;
  }

  inline Rectangle2DR getFromRectangle() const {
    return m_tr->getFromRectangle();
  }

  inline Rectangle2DR getToRectangle() const {
    return m_tr->getToRectangle();
  }

  inline void setFromRectangle(const Rectangle2DR &rect) {
    m_tr->setFromRectangle(rect);
  }
  
  void setToRectangle(const Rectangle2DR &rect);

  inline void setScale(IntervalScale newScale, int flags) {
    m_tr->setScale(newScale, flags);
  }

  void setRetainAspectRatio(bool retainAspectRatio);

  inline bool isRetainingAspectRatio() const {
    return m_retainAspectRatio;
  }

  inline const IntervalTransformation &getXTransformation() const {
    return m_tr->getXTransformation();
  }
  
  inline const IntervalTransformation &getYTransformation() const {
    return m_tr->getYTransformation();
  }
  
  inline Point2DP forwardTransform(const Point2DP &p) const {
    return m_tr->forwardTransform(p);
  }

  inline Point2DP forwardTransform(double x, double y) const {
    return m_tr->forwardTransform(x,y);
  }

  inline Point2DP backwardTransform(const Point2DP &p) const {
    return m_tr->backwardTransform(p);
  }

  inline Rectangle2DR forwardTransform(const Rectangle2DR &r) const {
    return m_tr->forwardTransform(r);
  }

  inline Rectangle2DR backwardTransform(const Rectangle2DR &r) const {
    return m_tr->backwardTransform(r);
  }

  void        setClipping(bool clip);
  void        MoveTo(      const Point2DP &p);
  void        MoveTo(double x, double y);
  void        LineTo(      const Point2DP &p);
  void        LineTo(double x, double y);
  void        SetPixel(    const Point2DP &p, COLORREF color);
  void        SetPixel(double x, double y, COLORREF color);
  void        paintCross(  const Point2DP &p, COLORREF color, int size=4);
  COLORREF    GetPixel(const Point2DP &p);
  COLORREF    GetPixel(double x, double y);
  bool        Rectangle(const Rectangle2DR &r);
  bool        Rectangle(double x1, double y1, double x2, double y2);
  void        FillSolidRect(const Rectangle2DR &r, COLORREF color);
  void        clear(COLORREF color);
  CGdiObject *SelectObject(CGdiObject *object);
  CBitmap    *SelectObject(CBitmap    *bitmap);
  CPen       *SelectObject(CPen       *pen);

  CGdiObject *SelectStockObject(int index);
  void paintDragRect(const Rectangle2D &rect, SIZE size, const Rectangle2D &lastRect, SIZE lastSize, CBrush *brush = NULL, CBrush *lastBrush = NULL);
};
