#pragma once

#include "PointGraph.h"
#include "DataGraphParameters.h"

class DataGraph : public PointGraph {
private:
  Point2D    strToPoint(const TCHAR *sx, const TCHAR *sy, const DataGraphParameters &param);
  __time64_t m_lastReadTime;
  bool       m_hasFirstDataPoint;
  Point2D    m_firstPoint;
  void readData();
  void readData(FILE *f);
public:
  DataGraph(CCoordinateSystem &system, DataGraphParameters &param);
  void refreshData();
  bool needRefresh() const;
  GraphType getType() const {
    return DATAGRAPH;
  }
};

