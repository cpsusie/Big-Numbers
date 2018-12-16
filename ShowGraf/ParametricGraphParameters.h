#pragma once

#include <MFCUtil/Viewport2D.h>
#include <Math/MathLib.h>
#include "GraphParameters.h"

class ParametricGraphParameters : public ExprGraphParameters {
private:
  String            m_commonText, m_exprX, m_exprY;
  DoubleInterval    m_interval;
  UINT              m_steps;
public:
  ParametricGraphParameters(const String &name=_T("Untitled"), COLORREF color=BLACK, UINT rollAvgSize=0, GraphStyle style=GSCURVE, TrigonometricMode trigonometricMode=RADIANS);
  void putDataToDoc(XMLDoc &doc);
  void getDataFromDoc(XMLDoc &doc);
  // return old Interval
  inline DoubleInterval setInteval(double from, double to) {
    const DoubleInterval oldInterval = m_interval; m_interval.setFrom(from).setTo(to); return oldInterval;
  }
  // return old Interval
  inline DoubleInterval setInteval(const DoubleInterval &interval) {
    const DoubleInterval oldInterval = m_interval; m_interval = interval; return oldInterval;
  }
  inline const DoubleInterval &getInterval() const {
    return m_interval;
  }
  // return old commonText
  inline String setCommonText(const String &text) {
    const String oldText = m_commonText; m_commonText = text; return oldText;
  }
  inline const String &getCommonText() const {
    return m_commonText;
  }

  // return old m_exprX
  inline String setExprXText(const String &text) {
    const String oldText = m_exprX; m_exprX = text; return oldText;
  }
  inline String getExprXText(bool prefixCommon) const {
    return prefixCommon ? (m_commonText + m_exprX) : m_exprX;
  }

  // return old m_exprY
  inline String setExprYText(const String &text) {
    const String oldText = m_exprY; m_exprY = text; return oldText;
  }
  inline String getExprYText(bool prefixCommon) const {
    return prefixCommon ? (m_commonText + m_exprY) : m_exprY;
  }
  // return old steps
  inline UINT setSteps(UINT steps) {
    const UINT oldSteps = m_steps; m_steps = steps; return oldSteps;
  }
  inline UINT getSteps() const {
    return m_steps;
  }
  int getType() const {
    return PARAMETRICGRAPH;
  }
};
