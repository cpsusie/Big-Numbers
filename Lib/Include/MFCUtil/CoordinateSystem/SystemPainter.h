#pragma once

#include <MFCUtil/Viewport2D.h>
#include <MFCUtil/Coordinatesystem/AbstractAxisPainter.h>

class CCoordinateSystem;

class SystemPainter {
private:
  CCoordinateSystem   &m_system;
  AbstractAxisPainter *m_axisPainter[2];
  CPoint               m_origin; // Point inside vp.toRectangle where axes cross. Space is left for textdata to the axes
  CFont               *m_font, *m_oldFont;
  bool                 m_mouseMode;
  void  makeSpaceForText();
  CRect getToRectangle() const;
  AbstractAxisPainter *createAxisPainter(AxisIndex axis, const AxisType type);
  inline void setMouseMode(bool mouseMode) {
    m_mouseMode = mouseMode;
  }
public:
  SystemPainter(CCoordinateSystem *system);
  ~SystemPainter();
  void                 paint();
  Viewport2D          &getViewport();
  const Viewport2D    &getViewport() const;
  const AxisAttribute &getAxisAttr(AxisIndex axis) const;
  const CPoint        &getOrigin() const;
  CSize                getTextExtent(const String &str);
  inline String        getValueText(AxisIndex axis, double value) const {
    return m_axisPainter[axis]->getValueText(value);
  }
  inline bool isMouseMode() const {
    return m_mouseMode;
  }
  inline String        getPointText(const Point2D &p) const {
    return format(_T("%s,%s"), getValueText(XAXIS_INDEX, p.x).cstr(), getValueText(YAXIS_INDEX,p.y).cstr());
  }
  void                 setOccupiedRect(const CRect &r);
  void                 setOccupiedLine(const CPoint &from, const CPoint &to);
};
