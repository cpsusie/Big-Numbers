#pragma once

#include <MFCUtil/Viewport2D.h>
#include "Graph.h"
#include "DataGraphParameters.h"

class DataGraph : public PointGraph {
private:
  Point2D tr1(const TCHAR *sx, const TCHAR *sy);
  bool    m_hasFirstDataPoint;
  Point2D m_firstPoint;
  void readData(FILE *f);
public:
  DataGraph(DataGraphParameters &param);
  
  GraphType getType() const {
    return DATAGRAPH;
  }
};

