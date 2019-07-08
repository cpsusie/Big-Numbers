#pragma once

#include <float.h>
#include "GraphParameters.h"
#include "MoveablePointArray.h"

#define EMPTY_DISTANCE 1e40

class GraphZeroesResult {
private:
  Graph                   &m_graph;
  const CompactDoubleArray m_zeroes;
public:
  GraphZeroesResult(Graph &graph, const CompactDoubleArray &zeroes)
    : m_graph(graph)
    , m_zeroes(zeroes)
  {
  }
  inline Graph &getGraph() const {
    return m_graph;
  }
  inline const CompactDoubleArray &getZeroes() const {
    return m_zeroes;
  }
  MoveablePointArray getMoveablePointArray() const;
  String toString(const TCHAR *name=NULL) const;
};

class GraphZeroesResultArray : public Array<GraphZeroesResult> {
private:
  Graph &m_graph;
public:
  inline GraphZeroesResultArray(Graph &graph) : m_graph(graph) {
  }
  inline Graph &getGraph() const {
    return m_graph;
  }
  MoveablePointArray getMoveablePointArray() const;
  String toString() const;
};

typedef enum {
  EXTREMA_TYPE_MAX
 ,EXTREMA_TYPE_MIN
} ExtremaType;

inline String toString(ExtremaType type) {
  return (type == EXTREMA_TYPE_MAX) ? _T("Maxima") : _T("Minima");
}

class GraphExtremaResult {
private:
  Graph             &m_graph;
  const ExtremaType  m_extremaType;
  const Point2DArray m_extrema;
public:
  GraphExtremaResult(Graph &graph, ExtremaType extremaType, const Point2DArray &extrema)
    : m_graph(graph)
    , m_extremaType(extremaType)
    , m_extrema(extrema)
  {
  }
  inline Graph &getGraph() const {
    return m_graph;
  }
  inline ExtremaType getExtremaType() const {
    return m_extremaType;
  }
  inline const Point2DArray &getExtrema() const {
    return m_extrema;
  }
  MoveablePointArray getMoveablePointArray() const;
  inline String getExtremaTypeName() const {
    return ::toString(getExtremaType());
  }
  String toString(const TCHAR *name=NULL) const;
};

class GraphExtremaResultArray : public Array<GraphExtremaResult> {
private:
  Graph            &m_graph;
  const ExtremaType m_extremaType;
public:
  inline GraphExtremaResultArray(Graph &graph, ExtremaType extremaType)
    : m_graph(graph)
    , m_extremaType(extremaType)
  {
  }
  inline Graph &getGraph() const {
    return m_graph;
  }
  inline ExtremaType getExtremaType() const {
    return m_extremaType;
  }
  inline String getExtremaTypeName() const {
    return ::toString(getExtremaType());
  }
  MoveablePointArray getMoveablePointArray() const;
  String toString() const;
};

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
  virtual void setRollAvgSize(UINT size) {
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

class PointGraph : public Graph {
private:
  Point2DArray         m_pointArray;
  mutable Point2DArray m_processedData;
  mutable bool         m_dataProcessed;
  DataRange            m_range;

public:
  PointGraph(CCoordinateSystem &system, GraphParameters *param);
  void paint(CDC &dc);

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
  void    setRollAvgSize(UINT size);
  double  distance(const CPoint &p) const;
  double  getSmallestPositiveX() const;
  double  getSmallestPositiveY() const;
  inline bool isPointGraph() const {
    return true;
  }
  GraphZeroesResultArray  findZeroes( const DoubleInterval &interval);
  GraphExtremaResultArray findExtrema(const DoubleInterval &interval, ExtremaType extremaType);
};
