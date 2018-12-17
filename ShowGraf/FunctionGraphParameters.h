#pragma once

#include <MFCUtil/Viewport2D.h>
#include <Math/MathLib.h>
#include "GraphParameters.h"

class FunctionGraphParameters : public ExprGraphParameters {
private:
  String            m_expr;
  DoubleInterval    m_interval;
  UINT              m_steps;
public:
  FunctionGraphParameters(const String &name=s_defaultName, COLORREF color=BLACK, UINT rollAvgSize=0, GraphStyle style=GSCURVE, TrigonometricMode trigonometricMode=RADIANS);
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

  // return old exprText
  inline String setExprText(const String &expr) {
    const String oldExpr = m_expr; m_expr = expr; return oldExpr;
  }
  inline const String &getExprText() const {
    return m_expr;
  }

  // return old steps
  inline UINT setSteps(UINT steps) {
    const UINT oldSteps = m_steps; m_steps = steps; return oldSteps;
  }
  inline UINT getSteps() const {
    return m_steps;
  }
  int getType() const {
    return FUNCTIONGRAPH;
  }
};

class FunctionPlotter {
public:
  virtual void plotFunction(Function &f, COLORREF color) = 0;
  virtual void plotFunction(Function &f, const DoubleInterval &interval, COLORREF color) = 0;
  virtual void addFunctionGraph(FunctionGraphParameters &param)      = 0;
};
