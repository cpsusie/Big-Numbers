#pragma once

#include <MFCUtil/Viewport2D.h>
#include <Math/MathLib.h>
#include "GraphParameters.h"

class ExpressionGraphParameters : public GraphParameters {
public:
  String            m_expr;
  DoubleInterval    m_interval;
  unsigned int      m_steps;
  TrigonometricMode m_trigonometricMode;
  ExpressionGraphParameters(const String &name=_T("Untitled"), COLORREF color=BLACK, int rollSize=0, GraphStyle style=GSCURVE, TrigonometricMode trigonomtetricMode=RADIANS);
  void read(const String &fileName);
  void write(FILE *f);
  void read( FILE *f);
};

class FunctionPlotter {
public:
  virtual void plotFunction(Function &f, COLORREF color) = 0;
  virtual void plotFunction(Function &f, const DoubleInterval &interval, COLORREF color) = 0;
  virtual void addExpressionGraph(ExpressionGraphParameters &param)      = 0;
};

