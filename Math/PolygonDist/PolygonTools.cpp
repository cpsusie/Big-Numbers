#include "StdAfx.h"
#include "PolygonTools.h"

double angle(const vector &v1, const vector &v2) {
  double a = acos(v1*v2 / v1.length() / v2.length());
  return a * sign(det(v1,v2));
}

double angle(const line &l1, const line &l2) {
  vector v1(l1.m_p1 - l1.m_p2);
  vector v2(l2.m_p1 - l2.m_p2);
  return angle(v1,v2);
}

void drawArrow(CDC &dc, const Point2DP start, const Point2DP end, int length) {
  const Point2D aStart(start);
  Point2D dirUnit(Point2D(end) - aStart);
  dirUnit.normalize();
  const Point2DP aEnd     =  aStart + dirUnit * length;
  const Point2D  dirUnit1 = -dirUnit;
  Point2DP vp1 = aEnd + dirUnit1.rotate(GRAD2RAD(+40)) * 6;
  Point2DP vp2 = aEnd + dirUnit1.rotate(GRAD2RAD(-40)) * 6;

  dc.MoveTo(start);
  dc.LineTo(aEnd);
  dc.LineTo(vp1);
  dc.MoveTo(aEnd);
  dc.LineTo(vp2);
}

void line::paint(CDC &dc, const CRect &rect, COLORREF color) const {
  vector normal(m_p2.y - m_p1.y, m_p1.x - m_p2.x );
  const int signx = sign(m_p2.x - m_p1.x);
  const int signy = sign(m_p2.y - m_p1.y);
  CPen pen;
  pen.CreatePen(PS_SOLID, 1, color);
  CPen *oldPen = dc.SelectObject(&pen);
  if(signx == 0) { // lodret linie
    dc.MoveTo(CPoint(m_p1.x,rect.top   ));
    dc.LineTo(CPoint(m_p1.x,rect.bottom));
  } else if(signy == 0) { // vandret linie
    dc.MoveTo(CPoint(rect.left ,m_p1.y));
    dc.LineTo(CPoint(rect.right,m_p1.y));
  } else {
    CPoint p1 = m_p1;
    while(rect.PtInRect(p1)) {
      CPoint pstepx(p1.x+signx,p1.y);
      CPoint pstepy(p1.x,p1.y+signy);
      if(abs((pstepx - m_p1) * normal) < abs((pstepy - m_p1) * normal))
        p1.x += signx;
      else
        p1.y += signy;
    }
    CPoint p2 = m_p1;
    while(rect.PtInRect(p2)) {
      CPoint pstepx(p2.x-signx,p2.y);
      CPoint pstepy(p2.x,p2.y-signy);
      if(abs((pstepx - m_p1) * normal) < abs((pstepy - m_p1) * normal))
        p2.x -= signx;
      else
        p2.y -= signy;
    }
    dc.MoveTo(p1);
    dc.LineTo(p2);
  }
  drawArrow(dc, m_p1, m_p2, 18);

  dc.SelectObject(oldPen);
}

CPoint polygon::getCenter() const {
  const int n = getPointCount();
  if(n == 0) return CPoint(0,0);
  CPoint sum = m_points[0];
  for (size_t i = 1; i < n; i++) {
    const CPoint &p = m_points[i];
    sum.x += p.x;
    sum.y += p.y;
  }
  sum.x /= n;
  sum.y /= n;
  return sum;
}

double polygon::pointInside(const CPoint &p) const { // 1=inside, -1=outside, 0=edge
  const size_t n = m_points.size();
  if(n < 3) return -1;
  vector v(p,m_points[0]);
  double d = 0;
  for(size_t i = 1; i <= n; i++) {
    const vector vnext(p,m_points[i%n]);
    d += angle(v,vnext);
    v = vnext;
  }
  return (abs(d) > 1) ? 1 : -1; // d always +/- 2PI or 0
}

void paintCross(CDC &dc, const CPoint &p, COLORREF color, int size) {
  CPen pen;
  pen.CreatePen(PS_SOLID, 1, color);
  CPen *oldPen = dc.SelectObject(&pen);
  dc.MoveTo(p.x-size,p.y-size);
  dc.LineTo(p.x+size,p.y+size);
  dc.MoveTo(p.x-size,p.y+size);
  dc.LineTo(p.x+size,p.y-size);
  dc.SelectObject(oldPen);
}

void polygon::paintVertex(CDC &dc, int index, const CPoint &center) const {
  const CPoint &p = m_points[index];
  paintCross(dc,p);
  const String str = format(_T("%d"), index);
  const CSize  tsz = getTextExtent(dc, str);
  const vector cp  = (getPointCount() < 2) ? vector(-1,0) : vector(center, p);
  const double l   = cp.length();
  const double tcx = p.x + (double)cp.m_p.x / l * tsz.cx/2;
  const double tcy = p.y + (double)cp.m_p.y / l * tsz.cy/2;
  const int    tx0 = (int)(tcx - tsz.cx/2);
  const int    ty0 = (int)(tcy - tsz.cy/2);
  textOut(dc, tx0, ty0, str);
}

void polygon::paint(CDC &dc) const {
  if(m_points.size() > 0) {
    const CPoint c = getCenter();
    paintVertex(dc,0, c);
    for(size_t i = 1; i < m_points.size(); i++) {
      paintVertex(dc,(int)i,c);
      dc.MoveTo(m_points[i-1]);
      dc.LineTo(m_points[i]);
    }
    dc.LineTo(m_points[0]);
  }
}

int polygon::findNearest(const CPoint &p) const {
  if(m_points.size() == 0) return -1;
  int current        = 0;
  double currentDist = dist(p,m_points[current]);
  for(size_t i = 1; i < m_points.size(); i++) {
    const double d = dist(p,m_points[i]);
    if(d < currentDist) {
      current = (int)i;
      currentDist = d;
    }
  }
  return current;
}

void polygon::removeNearest(const CPoint &p) {
  const int n = findNearest(p);
  if(n < 0) return;
  m_points.remove(n);
}

class DeterminantComparator : public Comparator<CPoint> {
private:
  const CPoint m_bottomPoint;
public:
  DeterminantComparator(const CPoint &bottomPoint) : m_bottomPoint(bottomPoint) {
  }
  AbstractComparator *clone() const {
    return new DeterminantComparator(m_bottomPoint);
  }
  int compare(const CPoint &p1, const CPoint &p2);
};

int DeterminantComparator::compare(const CPoint &p1, const CPoint &p2) {
  const vector v1(m_bottomPoint, p1);
  const vector v2(m_bottomPoint, p2);
  return sign(det(v2,v1));
}

void polygon::convexHull() {
  const int    lowest      = findBottomPoint();
  const CPoint bottomPoint = m_points[lowest];
  m_points.remove(lowest);
  m_points.sort(DeterminantComparator(bottomPoint));
  PointArray hull;
  hull.add(bottomPoint);
  hull.add(m_points[0]);
  for(size_t i = 1; i < m_points.size(); i++) {
    hull.add(m_points[i]);
    const CPoint &pi = hull.last();
    for(;;) {
      if(hull.size() <= 3) break;
      UINT j = (UINT)hull.size()-2;
      vector v1(hull[j-1],pi);
      vector v2(hull[j-1],hull[j]);
      vector v3(hull[j-1],hull[0]);
      if(sign(det(v2,v1)) != sign(det(v2,v3))) {
        hull.remove(j);
      } else {
        break;
      }
    }
  }
  m_points = hull;
}

void polygon::addPoint(const CPoint &p) {
  m_points.add(p);
  if(m_points.size() > 3) {
    convexHull();
  }
}

int polygon::findTopPoint() const {
  if(m_points.size() == 0) return -1;
  int current    = 0;
  int currentTop = m_points[current].y;
  for(size_t i = 1; i < m_points.size(); i++) {
    if(m_points[i].y < currentTop) {
      current = (int)i;
      currentTop = m_points[i].y;
    }
  }
  return current;
}

int polygon::findBottomPoint() const {
  if(m_points.size() == 0) return -1;
  int current    = 0;
  int currentTop = m_points[current].y;
  for(size_t i = 1; i < m_points.size(); i++) {
    if(m_points[i].y > currentTop) {
      current = (int)i;
      currentTop = m_points[i].y;
    }
  }
  return current;
}
