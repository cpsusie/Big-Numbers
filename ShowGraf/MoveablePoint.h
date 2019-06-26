#pragma once

#include <MFCUtil/Coordinatesystem/Coordinatesystem.h>

class Graph;

#define SHOWXCOORDINATE 0x1
#define SHOWYCOORDINATE 0x2
#define SHOWBOTHCOORDINATES (SHOWXCOORDINATE|SHOWYCOORDINATE)

typedef enum {
  MPT_UNDEFINED
 ,MPT_MINIMUM
 ,MPT_MAXIMUM
 ,MPT_ROOT
 ,MPT_LEFTENDPOINT
 ,MPT_RIGHTENDPOINT
} MoveablePointType;

class MoveablePoint : public CoordinateSystemObject {
private:
  const Graph            *m_graph;
  const MoveablePointType m_type;
  const DataRange         m_range;
  const BYTE              m_showFlags;
  Point2D                 m_location;
public:
  MoveablePoint(const Graph *graph, MoveablePointType type, const Point2D &point, BYTE showFlags = SHOWBOTHCOORDINATES)
    : m_graph(graph)
    , m_type(type)
    , m_range(point)
    , m_showFlags(showFlags)
    , m_location(point) {
  }
  inline const Graph *getGraph() const {
    return m_graph;
  }
  inline MoveablePointType getType() const {
    return m_type;
  }
  inline void setLocation(const Point2D &p) {
    m_location = p;
  }
  void paint(CCoordinateSystem &cs);
  const DataRange &getDataRange() const {
    return m_range;
  }
};
