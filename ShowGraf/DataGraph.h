#pragma once

#include <MFCUtil/Viewport2D.h>
#include "Graph.h"
#include "DataGraphParameters.h"

class DataGraph : public PointGraph {
private:
  Point2D tr1(const TCHAR *sx, const TCHAR *sy);
  __time64_t m_lastReadTime;
  bool       m_hasFirstDataPoint;
  Point2D    m_firstPoint;
  void readData();
  void readData(FILE *f);
public:
  DataGraph(DataGraphParameters &param);
  void refreshData();
  bool needRefresh() const;
  GraphType getType() const {
    return DATAGRAPH;
  }
};

