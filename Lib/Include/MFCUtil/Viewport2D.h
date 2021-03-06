#pragma once

#include <Math/Rectangle2DTransformation.h>
#include "WinTools.h"

class Viewport2D {
private:
  Rectangle2DTransformation *m_tr;
  bool                       m_ownTransformation;
  bool                       m_retainAspectRatio;

  mutable CDC             *m_dc;
  mutable CRgn             m_clipRgn;
  mutable CPen             m_currentPen;
  mutable int              m_currentPenStyle;
  mutable int              m_currentPenWidth;
  mutable COLORREF         m_currentPenColor;

  Viewport2D(const Viewport2D &src);            // not implemented
  Viewport2D &operator=(const Viewport2D &src); // not implemented

  CPen &setCurrentPen(int penStyle, int width, COLORREF color) const;
  inline void paintLine(int x1, int y1, int x2, int y2) const {
    m_dc->MoveTo(x1, y1); m_dc->LineTo(x2, y2);
  }
  void destroyClipRgn() const;
  void destroyTransformation();
  void destroyPen() const;
  inline bool hasDC() const {
    return m_dc && m_dc->m_hDC;
  }
#if defined(_DEBUG)
  void noDCError(const TCHAR *method) const;
#define CHECKHASDC() { if(!hasDC()) noDCError(__TFUNCTION__); }
#else
#define CHECKHASDC()
#endif  // _DEBUG

public:
  Viewport2D(bool retainAspectRatio = false);
  Viewport2D(CDC &dc, const Rectangle2D &from, const Rectangle2D &to, bool retainAspectRatio = false);
  Viewport2D(CDC &dc, Rectangle2DTransformation &tr,  bool retainAspectRatio = false);
  virtual ~Viewport2D();

  inline CDC *getDC() {
    return m_dc;
  }

  inline CDC *setDC(CDC *dc) const {
    CDC *old = m_dc; m_dc = dc; return old;
  }

  inline const Rectangle2DTransformation &getTransformation() const {
    return *m_tr;
  }
  inline       Rectangle2DTransformation &getTransformation() {
    return *m_tr;
  }

  inline void setFromRectangle(const Rectangle2D &rect) {
    getTransformation().setFromRectangle(rect);
  }
  inline Rectangle2D getFromRectangle() const {
    return getTransformation().getFromRectangle();
  }

  void setToRectangle(const Rectangle2D &rect);
  inline Rectangle2D getToRectangle() const {
    return getTransformation().getToRectangle();
  }

  inline void setScale(IntervalScale newScale, int flags) {
    getTransformation().setScale(newScale, flags);
  }

  void setRetainAspectRatio(bool retainAspectRatio);

  inline bool isRetainingAspectRatio() const {
    return m_retainAspectRatio;
  }

  inline const IntervalTransformation &getXTransformation() const {
    return getTransformation()[0];
  }
  inline const IntervalTransformation &getYTransformation() const {
    return getTransformation()[1];
  }

  inline Point2D forwardTransform(const Point2D &p) const {
    return getTransformation().forwardTransform(p);
  }
  inline Point2D forwardTransform(double x, double y) const {
    return forwardTransform(Point2D(x, y));
  }
  inline Point2D backwardTransform(const Point2D &p) const {
    return getTransformation().backwardTransform(p);
  }

  inline Rectangle2D forwardTransform(const Rectangle2D &r) const {
    return getTransformation().forwardTransform(r);
  }
  inline Rectangle2D backwardTransform(const Rectangle2D &r) const {
    return getTransformation().backwardTransform(r);
  }

         void     setClipping(bool         clip) const;
         void     MoveTo(    const Point2D  &p) const;
         void     LineTo(    const Point2D  &p) const;
         void     SetPixel(  const Point2D  &p, COLORREF color) const;
  inline void     MoveTo(    double x, double y) const {
    MoveTo(Point2D(x, y));
  }
  inline void     LineTo(    double x, double y) const {
    LineTo(Point2D(x, y));
  }
  inline void     SetPixel(  double x, double y, COLORREF color) const {
    SetPixel(Point2D(x, y),color);
  }
  void            paintCross(const Point2D  &p, COLORREF color, int size=4) const;
         COLORREF GetPixel(  const Point2D  &p) const;
  inline COLORREF GetPixel(  double x, double y) const {
    return GetPixel(Point2D(x, y));
  }
         bool     Rectangle( const Rectangle2D &r) const;
  inline bool     Rectangle( double x1, double y1, double x2, double y2) const {
    return Rectangle(Rectangle2D(Point2D(x1, y1), Point2D(x2, y2)));
  }
  void            FillSolidRect(const Rectangle2D &r, COLORREF color) const;
  // transparent background
  // If bckSave is specified, the bounding recangle of the written text is returned (can be used for selection)
  // and the original pixels in the bounding rectangle containing the text, will be saved, for later restore
  void            TextOut( const Point2D &p, const String &text, COLORREF color, BackgroundSaver *bckSave = nullptr) const;
  void            clear(COLORREF color) const;
  CGdiObject     *SelectObject(CGdiObject *object) const;
  CBitmap        *SelectObject(CBitmap    *bitmap) const;
  CPen           *SelectObject(CPen       *pen   ) const;
  CFont          *SelectObject(CFont      *font  ) const;

  CGdiObject     *SelectStockObject(   int index ) const;
  void paintDragRect(const Rectangle2D &rect, SIZE size, const Rectangle2D &lastRect, SIZE lastSize, CBrush *brush = nullptr, CBrush *lastBrush = nullptr) const;
};
