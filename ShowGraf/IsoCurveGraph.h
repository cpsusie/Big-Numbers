#pragma once

#include <MFCUtil/Viewport2D.h>
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
  IsoCurveGraph(IsoCurveGraphParameters &param);
  void             calculate();
  void             paint(Viewport2D &vp);
  bool             isEmpty() const;
  const DataRange &getDataRange() const;
  double           distance(const CPoint &p, const RectangleTransformation &tr) const;
  double           getSmallestPositiveX() const;
  double           getSmallestPositiveY() const;
  void             setTrigonometricMode(TrigonometricMode mode);
  GraphType        getType() const {
    return ISOCURVEGRAPH;
  }
  bool             isPointGraph() const {
    return false;
  }
};
