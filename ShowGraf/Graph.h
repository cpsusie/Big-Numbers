#pragma once

#include <float.h>
#include "GraphParameters.h"
#include "GraphZeroResult.h"
#include "GraphExtremaResult.h"

#define EMPTY_DISTANCE 1e40

class Graph : public CoordinateSystemObject {
private:
  bool             m_visible;
  GraphParameters *m_param;
  Graph(const Graph &src);            // Not implemented
  Graph &operator=(const Graph &src); // Not implemented
protected:
  Graph(CCoordinateSystem &system, GraphParameters *param) : CoordinateSystemObject(system) {
    m_param = param; TRACE_NEW(m_param);
  }
  static bool pointDefined(const Point2D &p) {
    return _finite(p.x) && !_isnan(p.x) && _finite(p.y) && !_isnan(p.y);
  }
  GraphZeroesResultArray makeZeroesResult(const CompactDoubleArray &zeroes) {
    GraphZeroesResultArray result(*this);
    if(!zeroes.isEmpty()) {
      result.add(GraphZeroesResult(*this, zeroes));
    }
    return result;
  }
  GraphExtremaResultArray makeExtremaResult(ExtremaType extremaType, const Point2DArray &extrema) {
    GraphExtremaResultArray result(*this, extremaType);
    if(!extrema.isEmpty()) {
      result.add(GraphExtremaResult(*this, extremaType, extrema));
    }
    return result;
  }
public:
  virtual ~Graph() {
    SAFEDELETE(m_param);
  }
  GraphParameters &getParam() const {
    return *m_param;
  }
  virtual void calculate() {}
  virtual bool isEmpty()      const = 0;
  virtual GraphType getType() const = 0;
  virtual double  distance(const CPoint &p) const = 0;
  static inline double getMinPositive(double x, double r) {
    return (x > 0 && (r == 0 || x < r)) ? x : r;
  }
  virtual double  getSmallestPositiveX() const = 0;
  virtual double  getSmallestPositiveY() const = 0;
  virtual void    setStyle(GraphStyle style) {
    m_param->setGraphStyle(style);
  }
  virtual void setRollingAvg(const RollingAvg &rollingAvg) {
  }
  virtual bool isPointGraph() const = 0;
  virtual void setVisible(bool visible) {
    m_visible = visible;
  }
  virtual bool isVisible() const {
    return m_visible;
  }
  virtual void refreshData() {
  }
  virtual bool needRefresh() const {
    return false;
  }
  virtual GraphZeroesResultArray findZeroes(const DoubleInterval &interval) = 0;
  // Find zero of line going through p1,p2
  // Assume y1 != y2
  static inline double inverseLinearInterpolate0(const Point2D &p1, const Point2D &p2) {
    return (p1.y*p2.x - p1.x*p2.y) / (p1.y-p2.y);
  }
  virtual GraphExtremaResultArray findExtrema(const DoubleInterval &interval, ExtremaType extremaType) = 0;
};
