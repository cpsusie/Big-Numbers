#pragma once

#include <Math/MathLib.h>
#include <Math/Rectangle2D.h>
#include <Math/Expression/Expression.h>
#include "ExprGraphParameters.h"

class IsoCurveGraphParameters : public ExprGraphParameters {
private:
  String           m_expr;
  Rectangle2D      m_boundingBox;
  double           m_cellSize;
public:
  IsoCurveGraphParameters(const String &name=s_defaultName, COLORREF color=BLACK, GraphStyle style=GSCURVE, TrigonometricMode trigonometricMode=RADIANS);
  virtual GraphParameters *clone() const {
    return new IsoCurveGraphParameters(*this);
  }
  void putDataToDoc(XMLDoc &doc);
  void getDataFromDoc(XMLDoc &doc);
  // return old exprText
  inline String setExprText(const String &expr) {
    const String oldExpr = expr; m_expr = expr; return oldExpr;
  }
  inline const String &getExprText() const {
    return m_expr;
  }
  inline const Rectangle2D &getBoundingBox() const {
    return m_boundingBox;
  }
  // return old boundingBox
  inline Rectangle2D setBoundingBox(const Rectangle2D &boundingBox) {
    const Rectangle2D oldbb = m_boundingBox; m_boundingBox = boundingBox; return oldbb;
  }
  inline double getCellSize() const {
    return m_cellSize;
  }
  // return old cellSize
  inline double setCellSize(double cellSize) {
    const double oldCellSize = m_cellSize; m_cellSize = cellSize; return oldCellSize;
  }
  GraphType getType() const {
    return ISOCURVEGRAPH;
  }
};
