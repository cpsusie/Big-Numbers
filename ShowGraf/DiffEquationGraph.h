#pragma once

#include <MFCUtil/Viewport2D.h>
#include <MFCUtil/ExpressionGraphics.h>
#include "Graph.h"
#include "DiffEquationGraphParameters.h"

class DiffEquationGraph : public Graph {
private:
  CompactArray<PointGraph*> m_pointGraphArray;
  DataRange                 m_dataRange;
  void clear();
  void updateDataRange();
  friend class DiffEquationHandler;
public:
  DiffEquationGraph(const DiffEquationGraphParameters &param);
  ~DiffEquationGraph();
  void calculate();
  void paint(CCoordinateSystem &cs);
  inline bool isEmpty() const {
    return m_pointGraphArray.isEmpty();
  }
  void setTrigoMode(TrigonometricMode mode);
  inline GraphType getType() const {
    return DIFFEQUATIONGRAPH;
  }
  const DataRange &getDataRange() const {
    return m_dataRange;
  }
  double distance(const CPoint &p,const RectangleTransformation &tr) const;
  double getSmallestPositiveX(void) const;
  double getSmallestPositiveY(void) const;
  bool   isPointGraph(void) const {
    return false;
  }
  void   setStyle(GraphStyle style);
  void   setVisible(bool visible);
  GraphZeroesResultArray  findZeroes( const DoubleInterval &interval) const;
  GraphExtremaResultArray findExtrema(const DoubleInterval &interval, ExtremaType extremaType) const;
};
