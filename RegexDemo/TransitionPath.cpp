#include "stdafx.h"
#include "DFAPainter.h"

#ifdef _DEBUG

String toString(const GridElement &ge) {
  return ge.d.m_occupied ? _T("1") : _T("0");
}

static MatrixDimension getGridSize(const CSize winSize) {
  Point2D e = Point2D(CIRCLE_RADIUS/3, CIRCLE_RADIUS/3);
  const int gridWidth  = (int)ceil((double)winSize.cx / e.x) + 4;
  const int gridHeight = (int)ceil((double)winSize.cy / e.y) + 4;
  return MatrixDimension(gridHeight, gridWidth);
}

TransitionGrid::TransitionGrid(const CSize winSize) 
: m_winSize(winSize)
, m_grid(getGridSize(winSize)) {

  m_tr.setFromRectangle(Rectangle2D(0,0,m_winSize.cx-1                   , m_winSize.cy                 - 1));
  m_tr.setToRectangle(Rectangle2D(  0,0,(double)m_grid.getColumnCount()-1, (double)m_grid.getRowCount() - 1));

  m_edgeMarkStep = 2;

  Point2D d1 = m_tr.backwardTransform(0,0);
  Point2D d2 = m_tr.backwardTransform(1,1);

  m_horzDist = (int)((d2.x - d1.x)   * 1000);
  m_vertDist = (int)((d2.y - d1.y)   * 1000);
  m_diagDist = (int)(distance(d1,d2) * 1000);

  m_distMatrix[1][1] = 0;
  m_distMatrix[1][0] = m_distMatrix[1][2] = m_vertDist;
  m_distMatrix[0][1] = m_distMatrix[2][1] = m_horzDist;
  m_distMatrix[0][0] = m_distMatrix[2][0] = m_distMatrix[0][2] = m_distMatrix[2][2] = m_diagDist;
}

void TransitionGrid::markStateCircles(const DFAPointArray &pointArray) {
  m_grid.clear();
  for(size_t i = 0; i < pointArray.size(); i++) {
    markCircle(pointArray[i]->getPosition());
  }
}

#define R2 (CIRCLE_RADIUS*CIRCLE_RADIUS)

void TransitionGrid::markCircle(int cx, int cy) {
  int x = CIRCLE_RADIUS;
  int y = 0;
  int gy[4] = { -1,-1,-1,-1};
  const IntervalTransformation &trX = m_tr.getXTransformation();

  while(x >= y) {
    CPoint gp = winToGrid(cx - x, cy - y);
    if(gp.y != gy[0]) {
      const int xr = (int)trX.forwardTransform(cx + x);
      for(;gp.x <= xr; gp.x++) getGridElement(gp).setOccupied();
      gy[0] = gp.y;
    }

    gp = winToGrid(cx - x, cy + y);
    if(gp.y != gy[1]) {
      const int xr = (int)trX.forwardTransform(cx + x);
      for(;gp.x <= xr; gp.x++) getGridElement(gp).setOccupied();
      gy[1] = gp.y;
    }

    gp = winToGrid(cx - y, cy - x);
    if(gp.y != gy[2]) {
      const int xr = (int)trX.forwardTransform(cx + y);
      for(;gp.x <= xr; gp.x++) getGridElement(gp).setOccupied();
      gy[2] = gp.y;
    }

    gp = winToGrid(cx - y, cy + x);
    if(gp.y != gy[3]) {
      const int xr = (int)trX.forwardTransform(cx + y);
      for(;gp.x <= xr; gp.x++) getGridElement(gp).setOccupied();
      gy[3] = gp.y;
    }

    if(abs((int)sqr(x-1) + (int)sqr(y)-R2) < abs((int)sqr(x) + (int)sqr(y+1)-R2)) {
      x--;
    } else {
      y++;
    }
  }
}

void TransitionGrid::initGrid() {
  for(size_t r = 0; r < m_grid.getRowCount(); r++) {
    for(size_t c = 0; c < m_grid.getColumnCount(); c++) {
      GridElement &e = m_grid(r,c);
      e.d.m_distance = 0;
      e.d.m_included = false;
    }
  }
}

String toString(const CPoint &p) {
  return format(_T("(%d,%d)"), p.x,p.y);
}

TransitionPath TransitionGrid::findShortestFreePath(const CPoint &from, const CPoint &to) {
  initGrid();
  CompactStack<CPoint> stack;
  const CPoint startPoint = winToGrid(from);
  GridElement &goal       = getGridElement(to.x, to.y);
  getGridElement(startPoint).setDistance(0);
  goal.setFree();

  CompactArray<CPoint> pa1(500), pa2(500);
  CompactArray<CPoint> *edge = &pa1, *newPoints = &pa2;
  edge->add(startPoint);
  while(!goal.isIncluded() && !edge->isEmpty()) {
    for(size_t i = 0; i < edge->size(); i++) {
      const CPoint      &gp = (*edge)[i];
      const GridElement &ge = getGridElement(gp);

      for(int dy = -1; dy <= 1; dy++) {
        for(int dx = -1; dx <= 1; dx++) {
          if(dx == 0 && dy == 0) continue;
          const CPoint np(gp.x+dx,gp.y+dy);
          if(!isInsideGrid(np)) continue;
          GridElement &ne = getGridElement(np);
          if(ne.isIncluded() || ne.isOccupied()) continue;
          ne.setDistance(getMindistance(np));
          newPoints->add(np);
        }
      }
    }
    edge->clear(-1);
    std::swap(edge,newPoints);
  }
  goal.setOccupied();

  if(!goal.isIncluded()) {
    throwException(_T("cannot connect (%d,%d) with (%d,%d)"), from.x, from.y, to.x, to.y);
  }
  getGridElement(startPoint).setFree();
  TransitionPath result;
  result.add(to);
  CPoint gp = winToGrid(to);
  for(;;) {
    gp = findNeighbourWithMinDistance(gp);
    if(gp == startPoint) {
      result.add(from);
      break;
    }
    result.add(gridToWin(gp));
  }
  return result;
};

CPoint TransitionGrid::findNeighbourWithMinDistance(const CPoint &gp) const {
  UINT minimum = -1;
  CPoint result;
  for(int dy = -1; dy <= 1; dy++) {
    for(int dx = -1; dx <= 1; dx++) {
      if(dx == 0 && dy == 0) continue;
      const CPoint np(gp.x+dx,gp.y+dy);
      if(!isInsideGrid(np)) continue;
      const GridElement &ne = getGridElement(np);
      if(ne.isIncluded() && (ne.getDistance() < minimum)) {
        result = np;
        minimum = ne.getDistance();
      }
    }
  }
  return result;
}

UINT TransitionGrid::getMindistance(const CPoint &gp) const {
  int minimum = -1;
  for(int dy = -1; dy <= 1; dy++) {
    for(int dx = -1; dx <= 1; dx++) {
      if(dx == 0 && dy == 0) continue;
      const CPoint np(gp.x+dx,gp.y+dy);
      if(!isInsideGrid(np)) continue;
      const GridElement &ne = getGridElement(np);
      if(!ne.isIncluded()) continue;
      int dist = ne.getDistance() + m_distMatrix[dy+1][dx+1];
      if((minimum < 0) || (dist < minimum)) {
        minimum = dist;
      }
    }
  }
  return minimum;
}

void TransitionGrid::markPathAsOccupied(const TransitionPath &path) {
  if (path.isEmpty()) return;
  for(size_t i = 1; i < path.size()-1; i += m_edgeMarkStep) {
    const CPoint &p = path[i];
    getGridElement(p.x,p.y).setOccupied();
  }
}

void TransitionGrid::unmarkPathAsOccupied(const TransitionPath &path) {
  if (path.isEmpty()) return;
  for(size_t i = 1; i < path.size()-1; i += m_edgeMarkStep) {
    const CPoint &p = path[i];
    getGridElement(p.x,p.y).setFree();
  }
}

String TransitionGrid::toString() const {
  const size_t rowCount = m_grid.getRowCount();
  const size_t colCount = m_grid.getColumnCount();

  String result;
  result = format(_T("WindowSize:(%d,%d)\nGridSize:(%u,%u)\n")
                 ,m_winSize.cx, m_winSize.cy
                 ,(UINT)colCount, (UINT)rowCount);

  for(size_t r = 0; r < rowCount; r++) {
    for(size_t c = 0; c < colCount; c++) {
      const GridElement &e = m_grid(r,c);
      result += e.isOccupied() ? _T('1'):_T('0');
    }
    result += _T("\n");
  }
  return result;
}

String TransitionGrid::distancesToString() const {
  const size_t rowCount = m_grid.getRowCount();
  const size_t colCount = m_grid.getColumnCount();
  String result;
  for(size_t r = 0; r < rowCount; r++) {
    for(size_t c = 0; c < colCount; c++) {
      const GridElement &e = m_grid(r,c);
      if(e.isIncluded()) {
        result += format(_T("%6d "), e.getDistance());
      } else {
        result += _T("       ");
      }
    }
    result += _T("\n");
  }
  return result;

}

#endif
