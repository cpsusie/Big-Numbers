#pragma once
#include "Graph.h"

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
  void    setRollingAvg(const RollingAvg   &rollingAvg);
  double  distance(     const CPoint       &p) const;
  double  getSmallestPositiveX() const;
  double  getSmallestPositiveY() const;
  inline bool isPointGraph() const {
    return true;
  }
  GraphZeroesResultArray  findZeroes(const DoubleInterval &interval);
  GraphExtremaResultArray findExtrema(const DoubleInterval &interval, ExtremaType extremaType);
};
