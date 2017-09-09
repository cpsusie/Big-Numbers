#pragma once

#include "Graph.h"
#include "IsoCurveFinder.h"
#include "IsoCurveGraphParameters.h"

class IsoCurveGraph : public Graph {
private:
  DataRange                 m_range;
  Point2DArray              m_pointArray;
  CompactArray<LineSegment> m_lineArray;
  friend class IsoCurveGraphEvaluator;
  void clear();
  void findDataRange();
public:
  IsoCurveGraph(const IsoCurveGraphParameters &param);
  void             calculate();
  void             paint(CCoordinateSystem &cs);
  bool             isEmpty() const;
  const DataRange &getDataRange() const;
  double           distance(const CPoint &p, const RectangleTransformation &tr) const;
  double           getSmallestPositiveX() const;
  double           getSmallestPositiveY() const;
  void             setTrigoMode(TrigonometricMode mode);
  GraphType        getType() const {
    return ISOCURVEGRAPH;
  }
  bool             isPointGraph() const {
    return false;
  }
  GraphZeroesResultArray findZeroes(const DoubleInterval &i) const;
};
