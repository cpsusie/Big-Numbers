#include "stdafx.h"
#include <MFCUtil/WinTools.h>
#include <MFCUtil/ShapeFunctions.h>

void applyToLine(const CPoint &p1, const CPoint &p2, PointOperator &op) {
  applyToLine(p1.x,p1.y,p2.x,p2.y,op);
}

void applyToLine(int x1, int y1, int x2, int y2, PointOperator &op) {
  if(x1 == x2) {
    if(y1 > y2) {
      swap(y1,y2);
    }
    for(CPoint p = CPoint(x1,y1); p.y <= y2; p.y++) {
      op.apply(p);
    }
  } else if(y1 == y2) {
    if(x1 > x2) {
      swap(x1,x2);
    }
    for(CPoint p = CPoint(x1,y1); p.x <= x2; p.x++) {
      op.apply(p);
    }
  } else {
    const int vx = x2 - x1;
    const int vy = y2 - y1;
    const int dx = sign(vx);
    const int dy = sign(vy);
    for(CPoint p = CPoint(x1,y1), p1 = CPoint(p.x-x1,p.y-y1);;) {
      op.apply(p);
      if(p.x == x2 && p.y == y2) {
        break;
      }
      if(abs((p1.x+dx)*vy - p1.y*vx) < abs(p1.x*vy - (p1.y+dy)*vx)) {
        p.x  += dx;
        p1.x += dx;
      } else {
        p.y  += dy;
        p1.y += dy;
      }
    }
  }
}

void applyToRectangle(const CRect &rect, PointOperator &op) {
  const CRect r = makePositiveRect(rect);
  if(r.left == r.right) {
    if(r.top == r.bottom) {
      op.apply(r.TopLeft());                                // rect contains only 1 pixel
    } else {
      applyToLine(r.left , r.top, r.left  , r.bottom , op); // rect is a vertical line
    }
  } else if(r.top == r.bottom) {
    applyToLine(r.left , r.top   , r.right, r.top    , op); // rect is a horizontal line
  } else {                                                  // rect is a "real" rectangle
    applyToLine(r.left , r.top   , r.right-1, r.top     , op); 
    applyToLine(r.right, r.top   , r.right  , r.bottom-1, op);
    applyToLine(r.right, r.bottom, r.left+1 , r.bottom  , op);
    applyToLine(r.left , r.bottom, r.left   , r.top+1   , op);
  }
}

void applyToFullRectangle(const CRect &rect, PointOperator &op) {
  const CRect r = makePositiveRect(rect);
  CPoint p;
  for(p.y = r.top; p.y < r.bottom; p.y++) {
    for(p.x = r.left; p.x < r.right; p.x++) {
      op.apply(p);
    }
  }
}

static void applyToEllipsePart(const CPoint &start, const CPoint &end, const CPoint &center, PointOperator &op) {
  const int    minx = min(start.x,end.x);
  const int    miny = min(start.y,end.y);
  const int    maxx = max(start.x,end.x);
  const int    maxy = max(start.y,end.y);
  const int    dx   = sign(end.x - start.x);
  const int    dy   = sign(end.y - start.y);
  const int    rx   = max(abs(start.x-center.x),abs(end.x-center.x));
  const int    ry   = max(abs(start.y-center.y),abs(end.y-center.y));
  const double rx2  = rx*rx;
  const double ry2  = ry*ry;
  const double rxy2 = rx2*ry2;

  if(rx == 0 || ry == 0) {
    return;
  }

  CPoint p  = start;
  CPoint pc = p - center;
  while(minx <= p.x && p.x <= maxx && miny <= p.y && p.y <= maxy) {
    if(p == end) {
      break;
    }
    op.apply(p);
    const double d1 = fabs(ry2*sqr(pc.x+dx) + rx2*sqr(pc.y   ) - rxy2);
    const double d2 = fabs(ry2*sqr(pc.x   ) + rx2*sqr(pc.y+dy) - rxy2);
    if(d1 < d2) {
      p.x  += dx;
      pc.x += dx;
    } else {
      p.y  += dy;
      pc.y += dy;
    }
  }
}

void applyToEllipse(const CRect &rect, PointOperator &op) {
  const CRect r = makePositiveRect(rect);
  const CPoint center = r.CenterPoint();
  applyToEllipsePart(CPoint(center.x,r.top   ),CPoint(r.right,center.y ),center, op);
  applyToEllipsePart(CPoint(r.right,center.y ),CPoint(center.x,r.bottom),center, op);
  applyToEllipsePart(CPoint(center.x,r.bottom),CPoint(r.left,center.y  ),center, op);
  applyToEllipsePart(CPoint(r.left,center.y  ),CPoint(center.x,r.top   ),center, op);
}

void applyToBezier(const Point2D &start, const Point2D &cp1, const Point2D &cp2, const Point2D &end, CurveOperator &op, bool applyStart) {
  Point2D p = start;
  if(applyStart) {
    op.apply(start);
  }
  for(float t = 0.1f; t < 1.01; t += 0.1f) {
    float tt = t*t;
    float s  = 1.0f - t;
    float ss = s*s;
    Point2D np = start*(ss*s) + cp1*(3*ss*t) + cp2*(3*s*tt) + end*(t*tt);
    op.apply(np);
    p = np;
  }
}

void CurveOperator ::apply(const Point2D &p) {
  if(m_firstTime) {
    m_currentPoint = p;
    m_firstTime = false;
  } else {
    line(m_currentPoint,p);
    m_currentPoint = p;
  }
}
