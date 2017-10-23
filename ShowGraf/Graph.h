#pragma once

#include <float.h>
#include <MFCUtil/Coordinatesystem/Coordinatesystem.h>
#include "GraphParameters.h"

#define EMPTY_DISTANCE 1e40

class GraphZeroesResult {
public:
  String             m_name;
  CompactDoubleArray m_zeroes;
  GraphZeroesResult(const String &name, const CompactDoubleArray &zeroes)
    : m_name(name)
    , m_zeroes(zeroes)
  {
  }
  String toString() const {
    return format(_T("Zeroes of %s:%s")
                 ,m_name.cstr(), m_zeroes.toStringBasicType().cstr());
  }
};

class GraphZeroesResultArray : public Array<GraphZeroesResult> {
public:
  String toString() const {
    return isEmpty() ? _T("No zeroes found") : __super::toString(_T("\n"));
  }
};

class Graph : public CoordinateSystemObject {
private:
  bool             m_visible;
  GraphParameters *m_param;
  Graph(const Graph &src);            // Not implemented
  Graph &operator=(const Graph &src); // Not implemented
protected:
  Graph(GraphParameters *param) {
    m_param = param; TRACE_NEW(m_param);
  }
  static bool pointDefined(const Point2D &p) {
    return _finite(p.x) && !_isnan(p.x) && _finite(p.y) && !_isnan(p.y);
  }
  GraphZeroesResultArray makeZeroesResult(const CompactDoubleArray &zeroes) const {
    GraphZeroesResultArray result;
    if(!zeroes.isEmpty()) {
      result.add(GraphZeroesResult(getParam().getDisplayName(), zeroes));
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
  virtual double  distance(const CPoint &p, const RectangleTransformation &tr) const = 0;
  inline double  distance(const Point2DP &p) const {
    return distance(p,RectangleTransformation::getId());
  }
  static inline double getMinPositive(double x, double r) {
    return (x > 0 && (r == 0 || x < r)) ? x : r;
  }
  virtual double  getSmallestPositiveX() const = 0;
  virtual double  getSmallestPositiveY() const = 0;
  virtual void    setStyle(GraphStyle style) {
    m_param->m_style = style;
  }
  virtual void setRollAvgSize(int size) {
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
  virtual GraphZeroesResultArray findZeroes(const DoubleInterval &i) const = 0;
  // Find zero of line going through p1,p2
  // Assume y1 != y2
  static inline double inverseLinearInterpolate0(const Point2D &p1, const Point2D &p2) {
    return (p1.y*p2.x - p1.x*p2.y) / (p1.y-p2.y);
  }
};

class PointGraph : public Graph {
private:
  Point2DArray         m_pointArray;
  mutable Point2DArray m_processedData;
  mutable bool         m_dataProcessed;
  DataRange            m_range;

public:
  PointGraph(GraphParameters *param);
  void paint(CCoordinateSystem &cs);

  inline bool isEmpty() const {
    return m_pointArray.isEmpty();
  }
  GraphType getType() const {
    return POINTGRAPH;
  }
  void addPoint(const Point2D &p);
  void clear();
  void updateDataRange();

  inline const DataRange &getDataRange() const {
    return m_range;
  }

  const Point2DArray &getProcessedData() const;

  inline const Point2DArray &getDataPoints() const {
    return m_pointArray;
  }
  void    setDataPoints(const Point2DArray &a);
  void    setRollAvgSize(int size);
  double  distance(const CPoint &p, const RectangleTransformation &tr) const;
  double  getSmallestPositiveX() const;
  double  getSmallestPositiveY() const;
  inline bool isPointGraph() const {
    return true;
  }
  GraphZeroesResultArray findZeroes(const DoubleInterval &i) const;
};
