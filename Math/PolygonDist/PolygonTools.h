#pragma once

#include <MFCUtil/ViewPort2D.h>

inline UINT dist2(const CPoint &p1, const CPoint &p2) {
  return sqr(p1.x-p2.x) + sqr(p1.y-p2.y);
}

inline double dist(const CPoint &p1, const CPoint &p2) {
  return sqrt(dist2(p1,p2));
}

void paintCross(CDC &dc, const CPoint &p, COLORREF color=0, int size=2);

class vector {
public:
  CPoint m_p;
  inline vector(const CPoint &p) : m_p(p) {
  }
  inline vector(int x, int y) : m_p(x,y) {
  }
  inline vector(const CPoint &p1, const CPoint &p2)
    : m_p(p2.x-p1.x, p2.y-p1.y)
  {
  }
  inline vector operator+(const vector &v) const {
    return vector(m_p.x + v.m_p.x, m_p.y + v.m_p.y);
  }
  inline vector operator-(const vector &v) const {
    return vector(m_p.x - v.m_p.x, m_p.y - v.m_p.y);
  }
  inline vector operator-() const {
    return vector(-m_p.x, -m_p.y);
  }
  inline double operator*(const vector &v) const {
    return m_p.x*v.m_p.x + m_p.y*v.m_p.y;
  }
  inline int sqrLength() const {
    return sqr(m_p.x) + sqr(m_p.y);
  }
  inline double length() const {
    return sqrt(sqrLength());
  }
};

double angle(const vector &v1, const vector &v2);

inline vector operator-(const CPoint &p1, const CPoint &p2) {
  return CPoint(p1.x - p2.x, p1.y - p2.y);
}

inline double det(const vector &v1, const vector &v2) {
  return v1.m_p.x * v2.m_p.y - v1.m_p.y * v2.m_p.x;
}

class line {
public:
  CPoint m_p1, m_p2;
  line(const CPoint &p1, const CPoint &p2) {
    m_p1 = p1;
    m_p2 = p2;
  }
  line(const CPoint &p , const vector &v) : m_p1(p) {
    m_p2.x = p.x + v.m_p.x;
    m_p2.y = p.y + v.m_p.y;
  }
  void paint(CDC &dc, const CRect &rect, COLORREF color) const;
};

double angle(const line &l1, const line &l2);

typedef CompactArray<CPoint> PointArray;

class polygon {
private:
  PointArray m_points;
  void paintVertex(CDC &dc, int index, const CPoint &center) const;
  CPoint getCenter() const;
public:
  inline void clear() {
    m_points.clear();
  }
  void paint(CDC &dc) const;
  void convexHull();
  void addPoint(         const CPoint &p);
  void removeNearest(    const CPoint &p);
  int  findTopPoint()    const;
  int  findBottomPoint() const;
  int  findNearest(      const CPoint &p) const;
  double pointInside(    const CPoint &p) const; // 1=inside, -1=outside, 0=edge
  inline int getPointCount() const {
    return (int)m_points.size();
  }
  inline const CPoint &point(int i) const {
    return m_points[i];
  }
};
