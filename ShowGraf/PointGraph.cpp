#include "stdafx.h"
#include <QueueList.h>
#include "Graph.h"

PointGraph::PointGraph(GraphParameters *param) : Graph(param) {
  m_dataProcessed = false;
}

void PointGraph::findDataRange() {
  if(isEmpty()) {
    m_range.init(-1,1,-1,1);
  } else {
    m_range.init(m_pointArray[0]);
    for(size_t i = 1; i < m_pointArray.size(); i++) {
      m_range.update(m_pointArray[i]);
    }
  }
}

const Point2DArray &PointGraph::getProcessedData() const {
  if(m_dataProcessed) {
    return m_processedData;
  }

  if(getParam().m_rollSize == 0) {
    m_processedData = m_pointArray;
  } else {
    m_processedData.clear();
    QueueList<double> queue;
    double sum = 0;
    const int maxQueueSize = getParam().m_rollSize;
    int n = m_pointArray.size();
    for(int i = 0; i < n; i++) {
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

void PointGraph::clear() {
  m_pointArray.clear();
  m_dataProcessed = false;
}

double PointGraph::distance(const CPoint &p, const RectangleTransformation &tr) const {
  const Point2DP tmpp(p);
  const Point2DArray &data = getProcessedData();
  int n = data.size();
  switch(getParam().m_style) {
  case GSCURVE:
    { double minDist = -1;
      for(int i = 1; i < n; i++) {
        const double dist = distanceFromLineSegment(tr.forwardTransform(data[i-1]),tr.forwardTransform(data[i]),tmpp);
        if(minDist < 0 || dist < minDist) {
          minDist = dist;
        }
      }
      return minDist;
    }
  case GSPOINT:
  case GSCROSS:
    { double minDist = -1;
      for(int i = 0; i < n; i++) {
        const double dist = ::distance(tr.forwardTransform(data[i]),tmpp);
        if(minDist < 0 || dist < minDist) {
          minDist = dist;
        }
      }
      return minDist;
    }
  default:
    throwException(_T("Invalid style:%d"), getParam().m_style);
  }
  return 0;
}

void PointGraph::setRollSize(int size) {
  if(size != getParam().m_rollSize) {
    m_dataProcessed = false;
  }
  m_param->m_rollSize = size;
}

void PointGraph::paint(Viewport2D &vp) {
  int   i;
  const Point2DArray &data  = getProcessedData();
  const int           n     = data.size();
  const COLORREF      color = getParam().m_color;

  switch(getParam().m_style) {
  case GSCURVE :
    if(n > 1) {
      CPen pen;
      pen.CreatePen(PS_SOLID, 1, color);
      vp.SelectObject(&pen);
      bool lastDefined = pointDefined(data[0]);
      if(lastDefined) {
        vp.MoveTo(data[0]);
      }
      for(i = 1; i < n; i++) {
        bool defined = pointDefined(data[i]);
        if(defined) {
          if(lastDefined) {
            vp.LineTo(data[i]);
          } else {
            vp.MoveTo(data[i]);
          }
        }
        lastDefined = defined;
      }
    }
    break;
  case GSPOINT :
    for(i = 0; i < n; i++) {
      if(pointDefined(data[i])) {
        vp.SetPixel(data[i], color);
      }
    }
    break;
  case GSCROSS :
    for(i = 0; i < n; i++) {
      if(pointDefined(data[i])) {
        vp.paintCross(data[i], color, 6);
      }
    }
    break;
  default:
    throwException(_T("Invalid style:%d"), getParam().m_style);
  }
}

double PointGraph::getSmallestPositiveX() const {
  const Point2DArray &data = getProcessedData();
  if(data.size() == 0) {
    return 0;
  } else {
    double result = max(0, data[0].x);
    for(size_t i = 0; i < data.size(); i++) {
      result = getMinPositive(data[i].x, result);
    }
    return result;
  }
}

double PointGraph::getSmallestPositiveY() const {
  const Point2DArray &data = getProcessedData();
  if(data.size() == 0) {
    return 0;
  } else {
    double result = max(0, data[0].y);
    for(size_t i = 0; i < data.size(); i++) {
      result = getMinPositive(data[i].y, result);
    }
    return result;
  }
}
