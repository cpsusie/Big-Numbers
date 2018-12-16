#include "stdafx.h"
#include <QueueList.h>
#include "Graph.h"

PointGraph::PointGraph(GraphParameters *param) : Graph(param) {
  m_dataProcessed = false;
}

void PointGraph::updateDataRange() {
  if(isEmpty()) {
    m_range.setToDefault();
  } else {
    m_range = m_pointArray;
  }
}

const Point2DArray &PointGraph::getProcessedData() const {
  if(m_dataProcessed) {
    return m_processedData;
  }

  if(getParam().getRollAvgSize() <= 1) {
    m_processedData = m_pointArray;
  } else {
    m_processedData.clear();
    QueueList<double> queue;
    double sum = 0;
    const size_t maxQueueSize = getParam().getRollAvgSize();
    const size_t n            = m_pointArray.size();
    for(size_t i = 0; i < n; i++) {
      if(queue.size() == maxQueueSize) {
        sum -= queue.get();
      }
      queue.put(m_pointArray[i].y);
      sum += m_pointArray[i].y;
      m_processedData.add(Point2D(m_pointArray[i].x,sum / queue.size()));
    }
  }
  m_dataProcessed = true;
  return m_processedData;
}

void PointGraph::addPoint(const Point2D &p) {
  m_pointArray.add(p);
  m_dataProcessed = false;
}

void PointGraph::setDataPoints(const Point2DArray &a) {
  m_pointArray = a;
  updateDataRange();
  m_dataProcessed = false;
}

void PointGraph::clear() {
  m_pointArray.clear();
  updateDataRange();
  m_dataProcessed = false;
}

double PointGraph::distance(const CPoint &p, const RectangleTransformation &tr) const {
  const Point2DP      tmpp(p);
  const Point2DArray &data = getProcessedData();
  const size_t        n    = data.size();
  switch(getParam().getGraphStyle()) {
  case GSCURVE:
    { double minDist = EMPTY_DISTANCE;
      for(size_t i = 1; i < n; i++) {
        try {
          const double dist = distanceFromLineSegment(tr.forwardTransform(data[i-1]),tr.forwardTransform(data[i]),tmpp);
          if(dist < minDist) {
            minDist = dist;
          }
        } catch (...) {
          // ignore
        }
      }
      return minDist;
    }
  case GSPOINT:
  case GSCROSS:
    { double minDist = EMPTY_DISTANCE;
      for(size_t i = 0; i < n; i++) {
        try {
          const double dist = ::distance(tr.forwardTransform(data[i]),tmpp);
          if(dist < minDist) {
            minDist = dist;
          }
        } catch(...) {
          // ignore
        }
      }
      return minDist;
    }
  default:
    throwException(_T("Invalid style:%d"), getParam().getGraphStyle());
  }
  return 0;
}

void PointGraph::setRollAvgSize(UINT size) {
  if(getParam().setRollAvgSize(size) != size) {
    m_dataProcessed = false;
  }
}

void PointGraph::paint(CCoordinateSystem &cs) {
  const Point2DArray &data  = getProcessedData();
  if(data.isEmpty()) return;
  const Point2D      *pp    = &data[0];
  const Point2D      *end   = &data.last();
  Viewport2D         &vp    = cs.getViewport();
  const COLORREF      color = getParam().getColor();
  Point2DArray        tmp;

  switch(getParam().getGraphStyle()) {
  case GSCURVE :
    if(data.size() > 1) {
      CPen pen;
      pen.CreatePen(PS_SOLID, 1, color);
      CPen *oldPen = vp.SelectObject(&pen);
      bool lastDefined = pointDefined(*pp);
      if(lastDefined) {
        vp.MoveTo(*pp);
        tmp.add(*pp);
      }
      while(pp++ < end) {
        const bool defined = pointDefined(*pp);
        if(defined) {
          if(lastDefined) {
            vp.LineTo(*pp);
            tmp.add(*pp);
          } else {
            vp.MoveTo(*pp);
            tmp.add(*pp);
          }
        } else {
          cs.setOccupiedConnectedPoints(tmp);
          tmp.clear(-1);
        }
        lastDefined = defined;
      }
      vp.SelectObject(oldPen);
      cs.setOccupiedConnectedPoints(tmp);
    }
    break;
  case GSPOINT :
    for(;pp <= end; pp++) {
      if(pointDefined(*pp)) {
        vp.SetPixel(*pp, color);
        tmp.add(*pp);
      }
    }
    cs.setOccupiedPoints(tmp);
    break;
  case GSCROSS :
    for(;pp <= end; pp++) {
      if(pointDefined(*pp)) {
        vp.paintCross(*pp, color, 6);
        tmp.add(*pp);
      }
    }
    cs.setOccupiedPoints(tmp);
    break;
  default:
    throwException(_T("Invalid style:%d"), getParam().getGraphStyle());
  }
}

double PointGraph::getSmallestPositiveX() const {
  const Point2DArray &data = getProcessedData();
  if(data.isEmpty()) return 0;
  double result = max(0, data[0].x);
  for(size_t i = 1; i < data.size(); i++) {
    result = getMinPositive(data[i].x, result);
  }
  return result;
}

double PointGraph::getSmallestPositiveY() const {
  const Point2DArray &data = getProcessedData();
  if(data.isEmpty()) return 0;
  double result = max(0, data[0].y);
  for(size_t i = 1; i < data.size(); i++) {
    result = getMinPositive(data[i].y, result);
  }
  return result;
}

static int point2DcmpX(const Point2D &p1, const Point2D &p2) {
  return sign(p1.x-p2.x);
}

GraphZeroesResultArray PointGraph::findZeroes(const DoubleInterval &interval) const {
  Point2DArray       data = getProcessedData();
  CompactDoubleArray result;
  if(data.isEmpty()) {
    return makeZeroesResult(result);
  }
  data.sort(point2DcmpX);
  const size_t n = data.size();
  if(n == 1) {
    const Point2D &p = data[0];
    if((p.y == 0) && interval.contains(p.x)) {
      result.add(p.x);
    }
    return makeZeroesResult(result);
  } else if (n == 2) {
    const Point2D &p0 = data[0];
    const Point2D &p1 = data[0];
    if(p0.y != p1.y) {
      const double x = inverseLinearInterpolate0(p0, p1);
      if(interval.contains(x)) {
        result.add(x);
      }
    } else {
      if((p0.y == 0) && interval.contains(p0.x)) result.add(p0.x);
      if((p1.y == 0) && interval.contains(p1.x)) result.add(p1.x);
    }
    return makeZeroesResult(result);
  }

  const Point2D *lastp      = &data[0];
  bool           lastInside = interval.contains(lastp->x);
  for(size_t t = 1; t < n; t++) {
    const Point2D &p       = data[t];
    const bool     pinside = interval.contains(p.x);
    if(sign(lastp->y) * sign(p.y) != 1) {
      if(lastp->y == 0) {
        if(lastInside) {
          result.add(lastp->x);
        }
      } else if(lastInside || pinside) { // && lastp->y != 0
        if(p.y != 0) { // lastp->y != 0 && p.y != 0 => opposite sign
          const double x = inverseLinearInterpolate0(p, *lastp);
          if(interval.contains(x)) {
            result.add(x);
          }
        }
      }
    }
    if(lastInside && !pinside) { // we're done
      lastInside = false;
      break;
    }
    lastp      = &p;
    lastInside = pinside;
  }
  if(lastInside && (lastp->y == 0)) {
    result.add(lastp->x);
  }
  return makeZeroesResult(result);
}

static inline bool isMoreExtreme(double y0, double y1, ExtremaType extremaType) {
  return (extremaType == EXTREMA_TYPE_MAX) ? (y1 > y0) : (y1 < y0);
}

GraphExtremaResultArray PointGraph::findExtrema(const DoubleInterval &interval, ExtremaType extremaType) const {
  Point2DArray data = getProcessedData();
  Point2DArray result;
  const size_t n = data.size();
  if(n != 0) {
    data.sort(point2DcmpX);
  }

  const Point2D *bestPoint = NULL;
  for(size_t t = 0; t < n; t++) {
    const Point2D &p = data[t];
    if(interval.contains(p.x)) {
      if((bestPoint == NULL) || isMoreExtreme(bestPoint->y, p.y, extremaType)) {
        bestPoint = &p;
      }
    }
  }
  if(bestPoint != NULL) {
    result.add(*bestPoint);
  }
  return makeExtremaResult(extremaType, result);
}
