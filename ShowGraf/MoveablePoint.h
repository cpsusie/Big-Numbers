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
  // in pixels relative to coordinateSystem-clientRect
  // filled by paint
  CBitmap                 m_saveBM;
  CRect                   m_textRect;
  inline void initTextRect() {
    m_textRect.left = m_textRect.top = m_textRect.right = m_textRect.bottom = 0;
  }
  inline bool hasTextRect() const {
    return getArea(m_textRect) != 0;
  }
public:
  MoveablePoint(Graph *graph, MoveablePointType type, const Point2D &point, BYTE showFlags = SHOWBOTHCOORDINATES);
  inline const Graph *getGraph() const {
    return m_graph;
  }
  inline MoveablePointType getType() const {
    return m_type;
  }
  inline void setLocation(const Point2D &p) {
    m_location = p;
  }
  inline const Point2D &getLocation() const {
    return m_location;
  }
  inline CPoint getTrLocation() const {
    return (Point2DP)getSystem().getTransformation().forwardTransform(getLocation());
  }
  inline void setTrLocation(const CPoint &p) {
    setLocation(getSystem().getTransformation().backwardTransform((Point2DP)p));
  }
  const CRect &getTextRect() const {
    return m_textRect;
  }
  void paint(  CDC &dc);
  void unpaint(CDC &dc);
  const DataRange &getDataRange() const {
    return m_range;
  }
};
