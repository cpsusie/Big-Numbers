#pragma once

#include <Math/MathLib.h>
#include <TinyBitSet.h>
#include "ExprGraphParameters.h"
#include "DiffEquationsystem.h"

typedef BitSet64 DiffEquationSet;

class EquationAttributes {
private:
  Real     m_startValue;
  bool     m_visible;
  COLORREF m_color;
public:
  EquationAttributes() : m_startValue(0), m_visible(false), m_color(0)  {
  }
  inline void setStartValue(Real startValue) {
    m_startValue = startValue;
  }
  inline Real getStartValue() const {
    return m_startValue;
  }
  inline void setVisible(bool visible) {
    m_visible = visible;
  }
  inline bool isVisible() const {
    return m_visible;
  }
  inline COLORREF getColor() const {
    return m_color;
  }
  inline void setColor(COLORREF color) {
    m_color = color;
  }
  String toString() const;
};

class DiffEquationGraphParameters : public ExprGraphParameters {
private:
  DiffEquationDescriptionArray     m_equationDescArray;
  CompactArray<EquationAttributes> m_attrArray;
  DoubleInterval                   m_interval; // x runs in this interval
  double                           m_maxError;
public:
  DiffEquationGraphParameters(const String &name=s_defaultName, GraphStyle style=GSCURVE, TrigonometricMode trigonometricMode=RADIANS);
  virtual GraphParameters *clone() const {
    return new DiffEquationGraphParameters(*this);
  }
  void addEquation(const DiffEquationDescription &desc, const EquationAttributes &attr) {
    m_equationDescArray.add(desc);
    m_attrArray.add(attr);
  }
  void removeAllEquations() {
    m_equationDescArray.clear();
    m_attrArray.clear();
  }
  const DiffEquationDescriptionArray &getEquationDescriptionArray() const {
    return m_equationDescArray;
  }
  DiffEquationDescriptionArray &getEquationDescriptionArray() {
    return m_equationDescArray;
  }
  const CompactArray<EquationAttributes> &getAttributeArray() const {
    return m_attrArray;
  }
  inline UINT getEquationCount() const {
    return (UINT)m_equationDescArray.size();
  }
  const DiffEquationDescription &getEquationDescription(UINT index) const {
    return m_equationDescArray[index];
  }
  const EquationAttributes &getEquationAttribute(UINT index) const {
    return m_attrArray[index];
  }

  DiffEquationSet getVisibleEquationSet() const;
  Vector getStartVector() const;
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
  // return old maxError
  inline double setMaxError(double maxError) {
    const double oldMax = m_maxError; m_maxError = maxError; return oldMax;
  }
  inline double getMaxError() const {
    return m_maxError;
  }
  int getType() const {
    return DIFFEQUATIONGRAPH;
  }
};
