#pragma once

#include <MFCUtil/Viewport2D.h>
#include <Math/MathLib.h>
#include "GraphParameters.h"

class ParametricGraphParameters : public GraphParameters {
protected:
  void readTextFile( FILE *f);
  void writeTextFile(FILE *f);

public:
  String            m_exprX, m_exprY;
  DoubleInterval    m_interval;
  UINT              m_steps;
  TrigonometricMode m_trigonometricMode;
  ParametricGraphParameters(const String &name=_T("Untitled"), COLORREF color=BLACK, int rollSize=0, GraphStyle style=GSCURVE, TrigonometricMode trigonomtetricMode=RADIANS);
};
