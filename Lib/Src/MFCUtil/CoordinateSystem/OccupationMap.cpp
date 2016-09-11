#include "stdafx.h"
#include <MFCUtil/ShapeFunctions.h>
#include <MFCUtil/Coordinatesystem/CoordinateSystem.h>

void OccupationMap::setCellSize(int cx, int cy) {
  if (cx == 0 || cy == 0) {
    return;
  }
  MatrixDimension gridDim;
  if (m_winSize.cx > 0) {
    gridDim.columnCount = (int)ceil((double)m_winSize.cx / cx);
  }
  if (m_winSize.cy > 0) {
    gridDim.rowCount = (int)ceil((double)m_winSize.cy / cy);
  }
  m_cellSize = CSize(cx, cy);
  setDimension(gridDim);
}

void OccupationMap::setWindowSize(const CSize &size) {
  if (size.cx == 0 || size.cy == 0) {
    return;
  }
  m_winSize = size;
  setCellSize(m_cellSize.cx, m_cellSize.cy);
}

void OccupationMap::setOccupiedPoint(const CPoint &p) {
  MatrixIndex i(p.y / m_cellSize.cy, p.x / m_cellSize.cx);
  if(getDimension().isLegalIndex(i)) {
    set(i, true);
  }
}

class SetOccupied : public PointOperator {
private:
  OccupationMap &m_grid;
public:
  SetOccupied(OccupationMap *grid) : m_grid(*grid) {
  }
  void apply(const CPoint &p);
};

void SetOccupied::apply(const CPoint &p) {
  m_grid.set(p.y, p.x,true);
}

void OccupationMap::setOccupiedLine(const CPoint &from, const CPoint &to) {
  const CPoint p1(from.x / m_cellSize.cx, from.y / m_cellSize.cy);
  const CPoint p2(to.x   / m_cellSize.cx, to.y   / m_cellSize.cy);
  applyToLine(p1,p2, SetOccupied(this));
}

void OccupationMap::setOccupiedRect(const CRect &r) {
  const CRect  wr = getRect();

  CPoint tl = r.TopLeft();     tl.x = max(tl.x,0);             tl.y = max(tl.y,0);
  CPoint br = r.BottomRight(); br.x = min(br.x, m_winSize.cx); br.y = min(br.y, m_winSize.cy);
  if(tl.x > br.x) ::swap(tl.x,br.x); // make sure we have the right orientation, no matter how r is
  if(tl.y > br.y) ::swap(tl.y,br.y);

  const MatrixIndex itl(tl.y / m_cellSize.cy, tl.x / m_cellSize.cx);
  const MatrixIndex ibr(br.y / m_cellSize.cy, br.x / m_cellSize.cx);
  for (size_t r = itl.r; r <= ibr.r; r++) {
    for (size_t c = itl.c; c <= ibr.c; c++) {
      set(r,c,true);
    }
  }
}

PointArray &OccupationMap::createDistinctPointArray(PointArray &dst, const PointArray &src) const {
  const CPoint *p   = &src[0];
  const CPoint *end = &src.last();
  CPoint        last((int)(p->x / m_cellSize.cx), (int)(p->y / m_cellSize.cy));
  dst.add(CPoint(last.x*m_cellSize.cx,last.y*m_cellSize.cy));
  while (p++ < end) {
    const CPoint next((int)(p->x / m_cellSize.cx), (int)(p->y / m_cellSize.cy));
    if (next != last) {
      last = next;
      dst.add(CPoint(last.x*m_cellSize.cx,last.y*m_cellSize.cy));
    }
  }
  return dst;
}

void OccupationMap::setOccupiedPoints(const PointArray &pointArray) {
  if(pointArray.size() == 0) return;
  PointArray tmp;
  createDistinctPointArray(tmp, pointArray); // pointArray.size() >= 1 => tmp.size() >= 1

  for(const CPoint *p = &tmp[0], *end = &tmp.last(); p <= end;) {
    setOccupiedPoint(*(p++));
  }
}

void OccupationMap::setOccupiedConnectedPoints(const PointArray &pointArray) {
  if(pointArray.size() <= 1) return;
  PointArray tmp;
  createDistinctPointArray(tmp, pointArray); // pointArray.size() > 1 => tmp.size() >= 1

  const CPoint *last = &tmp[0];
  if(tmp.size() == 1) {
    set(last->y, last->x, true);
  } else {
    const CPoint *end = &tmp.last();
    for (const CPoint *next = last; next++ < end; last = next) {
      setOccupiedLine(*last, *next);
    }
  }
}
