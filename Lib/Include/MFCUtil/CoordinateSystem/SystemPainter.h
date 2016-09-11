#pragma once

#include <MFCUtil/Viewport2D.h>
#include <MFCUtil/Coordinatesystem/AbstractAxisPainter.h>

class CCoordinateSystem;

class SystemPainter {
private:
  CCoordinateSystem   &m_system;
  AbstractAxisPainter *m_xAxisPainter;
  AbstractAxisPainter *m_yAxisPainter;
  CPoint               m_origin; // Point inside vp.toRectangle where axes cross. Space is left for textdata to the axes
  CFont               *m_font, *m_oldFont;

  void  makeSpaceForText();
  CRect getToRectangle() const;
  AbstractAxisPainter *createAxisPainter(bool xAxis, AxisType axisType);
public:
  SystemPainter(CCoordinateSystem *system);
  ~SystemPainter();
  void              paint();
  Viewport2D       &getViewport();
  const Viewport2D &getViewport() const;
  COLORREF          getAxisColor() const;
  bool              hasGrid() const;
  const CPoint     &getOrigin() const;
  CSize             getTextExtent(const String &str);
  void              setOccupiedRect(const CRect &r);
  void              setOccupiedLine(const CPoint &from, const CPoint &to);
};
