#pragma once

#include <MFCUtil/Viewport2D.h>
#include <Math/MathLib.h>
#include "GraphParameters.h"

class ParametricGraphParameters : public GraphParameters {
public:
  String            m_commonText, m_exprX, m_exprY;
  DoubleInterval    m_interval;
  UINT              m_steps;
  TrigonometricMode m_trigonometricMode;
  ParametricGraphParameters(const String &name=_T("Untitled"), COLORREF color=BLACK, int rollAvgSize=0, GraphStyle style=GSCURVE, TrigonometricMode trigonomtetricMode=RADIANS);
  void putDataToDoc(XMLDoc &doc);
  void getDataFromDoc(XMLDoc &doc);
  int getType() const {
    return PARAMETRICGRAPH;
  }
};
