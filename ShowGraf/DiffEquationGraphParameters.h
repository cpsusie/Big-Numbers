#pragma once

#include <MFCUtil/Viewport2D.h>
#include <Math/MathLib.h>
#include <TinyBitSet.h>
#include "GraphParameters.h"
#include "DiffEquationsystem.h"

typedef BitSet64 DiffEquationSet;

class EquationAttributes {
public:
  Real     m_startValue;
  bool     m_visible;
  COLORREF m_color;
  EquationAttributes() : m_startValue(0), m_visible(false), m_color(0)  {
  }
  String toString() const;
};

class DiffEquationGraphParameters : public GraphParameters {
public:
  DiffEquationSystemDescription    m_equationsDescription;
  CompactArray<EquationAttributes> m_attrArray;
  DoubleInterval                   m_interval;
  double                           m_eps;
  TrigonometricMode                m_trigonometricMode;
  DiffEquationGraphParameters(const String &name=_T("Untitled"), GraphStyle style=GSCURVE, TrigonometricMode trigonomtetricMode=RADIANS);
  UINT getEquationCount() const {
    return (UINT)m_equationsDescription.size();
  }
  DiffEquationSet getVisibleEquationSet() const;
  Vector getStartVector() const;
  void putDataToDoc(XMLDoc &doc);
  void getDataFromDoc(XMLDoc &doc);
  int getType() const {
    return DIFFEQUATIONGRAPH;
  }
};
