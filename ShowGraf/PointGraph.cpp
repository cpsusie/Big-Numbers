#include "stdafx.h"
#include <QueueList.h>
#include "Graph.h"

PointGraph::PointGraph(CCoordinateSystem &system, GraphParameters *param)
: Graph(system, param)
{
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

  if(!getParam().getRollingAvg().isEnabled()) {
    m_processedData = m_pointArray;
  } else {
    m_processedData.clear();
    QueueList<double> queue;
    double sum = 0;
    const size_t maxQueueSize = getParam().getRollingAvg().getQueueSize();
    for(const Point2D p : m_pointArray) {
      if(queue.size() == maxQueueSize) {
        sum -= queue.get();
      }
      queue.put(p.y());
      sum += p.y();
      m_processedData.add(Point2D(p.x(),sum / queue.size()));
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

double PointGraph::distance(const CPoint &p) const {
  const  Point2D                  tmpp(p);
  const  Point2DArray            &data    = getProcessedData();
  const  RectangleTransformation &tr      = getSystem().getTransformation();
  double                          minDist = EMPTY_DISTANCE;
  switch(getParam().getGraphStyle()) {
  case GSCURVE:
    { UINT    count = 0;
      Point2D plast, pnew;
      for(Point2D p : data) {
        try {
          if(count == 0) {
            pnew = tr.forwardTransform(p);
          } else {
            plast = pnew;
            pnew  = tr.forwardTransform(p);
            const double dist = distanceFromLineSegment(plast,pnew,tmpp);
            if(dist < minDist) {
              minDist = dist;
              if(minDist == 0) {
                break;
              }
            }
          }
          count++;
        } catch (...) {
          // ignore
        }
      }
    }
    break;
  case GSPOINT:
  case GSCROSS:
    { for(Point2D p : data) {
        try {
          const double dist = ::distance(tr.forwardTransform(p),tmpp);
          if(dist < minDist) {
            minDist = dist;
            if(minDist == 0) {
              break;
            }
          }
        } catch(...) {
          // ignore
        }
      }
    }
    break;
  default:
    throwException(_T("Invalid style:%d"), getParam().getGraphStyle());
  }
  return minDist;
}

void PointGraph::setRollingAvg(const RollingAvg &rollingAvg) {
  if(getParam().setRollingAvg(rollingAvg) != rollingAvg) {
    m_dataProcessed = false;
  }
}

void PointGraph::paint(CDC &dc) {
  const Point2DArray &data  = getProcessedData();
  if(data.isEmpty()) return;
  const Point2D      *pp     = (Point2D*)data.begin();
  const Point2D      *endp   = (Point2D*)data.end();
  CCoordinateSystem  &system = getSystem();
  Viewport2D         &vp     = system.getViewport();
  CDC                *oldDC  = vp.setDC(&dc);
  const COLORREF      color  = getParam().getColor();
  Point2DArray        tmp;

  try {
    switch (getParam().getGraphStyle()) {
    case GSCURVE:
      if(data.size() > 1) {
        CPen pen;
        pen.CreatePen(PS_SOLID, 1, color);
        CPen *oldPen = vp.SelectObject(&pen);
        bool lastDefined = pointDefined(*pp);
        if(lastDefined) {
          vp.MoveTo(*pp);
          tmp.add(*pp);
        }
        for(pp++; pp < endp; pp++) {
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
            system.setOccupiedConnectedPoints(tmp);
            tmp.clear(-1);
          }
          lastDefined = defined;
        }
        vp.SelectObject(oldPen);
        system.setOccupiedConnectedPoints(tmp);
      }
      break;
    case GSPOINT:
      for(; pp < endp; pp++) {
        if(pointDefined(*pp)) {
          vp.SetPixel(*pp, color);
          tmp.add(*pp);
        }
      }
      system.setOccupiedPoints(tmp);
      break;
    case GSCROSS:
      for(; pp < endp; pp++) {
        if(pointDefined(*pp)) {
          vp.paintCross(*pp, color, 6);
          tmp.add(*pp);
        }
      }
      system.setOccupiedPoints(tmp);
      break;
    default:
      throwException(_T("Invalid style:%d"), getParam().getGraphStyle());
    }
    vp.setDC(oldDC);
  } catch(...) {
    vp.setDC(oldDC);
    throw;
  }
}

double PointGraph::getSmallestPositiveX() const {
  const Point2DArray &data = getProcessedData();
  if(data.isEmpty()) {
    return 0;
  }

  double result = max(0, data.first()[0]);
  for(const Point2D p : data) {
    result = getMinPositive(p[0], result);
  }
  return result;
}

double PointGraph::getSmallestPositiveY() const {
  const Point2DArray &data = getProcessedData();
  if(data.isEmpty()) {
    return 0;
  }
  double result = max(0, data.first()[1]);
  for(const Point2D p : data) {
    result = getMinPositive(p[1], result);
  }
  return result;
}

static int point2DcmpX(const PointTemplate<double,2> &p1, const PointTemplate<double,2> &p2) {
  return sign(p1[0]-p2[0]);
}

GraphZeroesResultArray PointGraph::findZeroes(const DoubleInterval &interval) {
  Point2DArray       data = getProcessedData();
  CompactDoubleArray result;
  if(data.isEmpty()) {
    return makeZeroesResult(result);
  }
  data.sort(point2DcmpX);
  const size_t n = data.size();
  if(n == 1) {
    const Point2D &p = data.first();
    if((p.y() == 0) && interval.contains(p.x())) {
      result.add(p.x());
    }
    return makeZeroesResult(result);
  } else if(n == 2) {
    const Point2D &p0 = data[0], &p1 = data[1];
    if(p0.y() != p1.y()) {
      const double x = inverseLinearInterpolate0(p0, p1);
      if(interval.contains(x)) {
        result.add(x);
      }
    } else {
      if((p0.y() == 0) && interval.contains(p0.x())) result.add(p0.x());
      if((p1.y() == 0) && interval.contains(p1.x())) result.add(p1.x());
    }
    return makeZeroesResult(result);
  }

  const Point2D *lastp      = (Point2D*)data.begin();
  bool           lastInside = interval.contains(lastp->x());
  for(size_t t = 1; t < n; t++) {
    const Point2D &p       = data[t];
    const bool     pinside = interval.contains(p.x());
    if(sign(lastp->y()) * sign(p.y()) != 1) {
      if(lastp->y() == 0) {
        if(lastInside) {
          result.add(lastp->x());
        }
      } else if(lastInside || pinside) { // && lastp->y != 0
        if(p.y() != 0) { // lastp->y != 0 && p.y != 0 => opposite sign
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
  if(lastInside && (lastp->y() == 0)) {
    result.add(lastp->x());
  }
  return makeZeroesResult(result);
}

static inline bool isMoreExtreme(double y0, double y1, ExtremaType extremaType) {
  return (extremaType == EXTREMA_TYPE_MAX) ? (y1 > y0) : (y1 < y0);
}

GraphExtremaResultArray PointGraph::findExtrema(const DoubleInterval &interval, ExtremaType extremaType) {
  Point2DArray data = getProcessedData();
  Point2DArray result;
  const size_t n = data.size();
  if(n != 0) {
    data.sort(point2DcmpX);
  }

  Point2D bestPoint;
  bool    found = false;
  for(const Point2D p : data) {
    if(interval.contains(p.x())) {
      if(!found || isMoreExtreme(bestPoint.y(), p.y(), extremaType)) {
        bestPoint = p;
        found     = true;
      }
    }
  }
  if(found) {
    result.add(bestPoint);
  }
  return makeExtremaResult(extremaType, result);
}
