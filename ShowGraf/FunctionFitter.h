#pragma once

#include <Math/Expression/Expression.h>
#include <Math/Matrix.h>
#include "FunctionGraph.h"

class FunctionFitter : public Expression, public Function {
private:
  const Point2DArray    m_data;
  StringArray           m_parameterName;
  CompactArray<double*> m_parameterValue;
  double               *m_x;
  double                m_SSD, m_lastSSD, m_SSDDescent;
  int                   m_iteration;
  bool                  m_done;
  Vector                m_gradient;
  double                m_stepSize;

  void   randomVarList();
  double computeSSD();
  double d2SSD_dx2(const Vector &gradient, double &dydx, double x0);
  double findStepSize();
  void   dumpVarList(FILE *f = stdout);
  void   plotSSD();
public:
  FunctionFitter(const String &expr, const Point2DArray &data);
  double operator()(const double &x);

  double computeGradient(Vector &gradient);
  void   stepIteration();
  void   init();
  void   solve();
  double eval(double x);
  String toString() const;
  
  bool done() const {
    return m_done;
  }

  Vector getParam() const;
  void setParam(const Vector &values);
  
  int getParamCount() const {
    return (int)m_parameterValue.size();
  }
  
  const String &getParamName(size_t i) const {
    return m_parameterName[i];
  }
  
  double getParamValue(size_t i) const {
    return *m_parameterValue[i];
  }
  
  void setParamValue(size_t i, double value) {
    *m_parameterValue[i] = value;
  }
  
  double getStepSize() const {
    return m_stepSize;
  }
  
  double getSSD() const {
    return m_SSD;
  }
  
  double getSSDDescent() const {
    return m_SSDDescent;
  }
  
  int getIterationCount() const {
    return m_iteration;
  }

  void terminate() {
    m_done = true;
  }
  
  const Point2DArray &getData() const {
    return m_data;
  }
};

