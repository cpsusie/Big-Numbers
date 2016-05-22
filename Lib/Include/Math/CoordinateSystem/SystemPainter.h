#pragma once

#include <Math/Viewport2D.h>
#include <Math/Coordinatesystem/AbstractAxisPainter.h>

class SystemPainter {
private:
  AbstractAxisPainter *m_xAxisPainter;
  AbstractAxisPainter *m_yAxisPainter;
  Viewport2D          &m_vp;
  CPoint               m_origin; // Point inside vp.toRectangle where axes cross. Space is left for textdata to the axes
  CFont               *m_font, *m_oldFont;
  COLORREF             m_backgroundColor;
  COLORREF             m_axisColor;
  bool                 m_grid;

  void  makeSpaceForText();
  CRect getToRectangle() const;
  void  initOrigin();
  AbstractAxisPainter *createAxisPainter(bool xAxis, AxisType axisType);
public:
  SystemPainter(Viewport2D &vp, CFont *font, COLORREF backgroundColor, COLORREF axisColor, AxisType xAxisType, AxisType yAxisType, bool grid);
  ~SystemPainter();
  void          paint();
  Viewport2D   &getViewport();
  COLORREF      getAxisColor() const;
  bool          hasGrid() const;
  const CPoint &getOrigin() const;
  CSize         getTextExtent(const String &str);
};
